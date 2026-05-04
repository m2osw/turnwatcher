import { useCallback, useEffect } from 'react'
import { useAppSelector, useAppDispatch, store } from './store'
import { ActionCreators } from 'redux-undo'
import {
  setCharacters, clearCharacters, purgeDead,
  removeCharacter, duplicateCharacter, stabilizeCharacter,
  setCharacterStatus, setStatValue,
} from './store/slices/charactersSlice'
import { setInitiativeState, startRounds, endRounds, nextTurn } from './store/slices/initiativeSlice'
import { sortByInitiative } from './utils/initiative'
import { setStats } from './store/slices/statsSlice'
import { updateSettings } from './store/slices/settingsSlice'
import { setStatusMessage, openDialog } from './store/slices/uiSlice'
import { forceAutosave } from './store/middleware/autosaveMiddleware'
import { Toolbar } from './components/MainWindow/Toolbar'
import { CharacterTable } from './components/CharacterView/CharacterTable'
import { StatusBar } from './components/MainWindow/StatusBar'
import { EditCharacterDialog } from './components/EditCharacter/EditCharacterDialog'
import { DamageDialog } from './components/DamageDialog/DamageDialog'
import { InitRollDialog } from './components/InitRollDialog/InitRollDialog'
import { AboutDialog } from './components/AboutDialog/AboutDialog'
import { PreferencesDialog } from './components/Preferences/PreferencesDialog'
import { EffectsEditorDialog } from './components/EffectsEditor/EffectsEditorDialog'
import { StatManagerDialog } from './components/StatManager/StatManagerDialog'
import { SaveFile, AppSettings, Status } from './types'
import { peekNextInit } from './utils/initiative'
import { makeStatRoll } from './utils/dice'
import { getStatusString } from './utils/health'

export default function App() {
  const dispatch = useAppDispatch()
  const openDialogType = useAppSelector(state => state.ui.openDialog)
  const hash = window.location.hash

  // ========== Restore autosave on startup ==========

  useEffect(() => {
    async function restoreAutosave() {
      if (!window.electronAPI?.autosaveRead) return
      const data = await window.electronAPI.autosaveRead()
      if (!data) return

      try {
        const saveFile: SaveFile = JSON.parse(data)
        dispatch(setCharacters(saveFile.characters))
        dispatch(setInitiativeState(saveFile.initiative))
        if (saveFile.stats) {
          dispatch(setStats(saveFile.stats))
        }
        if (saveFile.settings) {
          dispatch(updateSettings(saveFile.settings as Partial<AppSettings>))
        }
        dispatch(ActionCreators.clearHistory())
        dispatch(setStatusMessage('Restored previous session'))
      } catch {
        // Autosave was corrupted — start fresh
        dispatch(setStatusMessage('Ready'))
      }
    }

    restoreAutosave()
  }, [dispatch])

  // ========== Force autosave on window close / beforeunload ==========

  useEffect(() => {
    function handleBeforeUnload() {
      forceAutosave(store.getState())
    }
    window.addEventListener('beforeunload', handleBeforeUnload)
    return () => window.removeEventListener('beforeunload', handleBeforeUnload)
  }, [])

  // ========== File Import handler ==========

  const handleFileImport = useCallback(async () => {
    if (!window.electronAPI) return
    const result = await window.electronAPI.fileImport()
    if (!result) return

    try {
      const saveFile: SaveFile = JSON.parse(result.data)
      dispatch(setCharacters(saveFile.characters))
      dispatch(setInitiativeState(saveFile.initiative))
      if (saveFile.stats) {
        dispatch(setStats(saveFile.stats))
      }
      if (saveFile.settings) {
        dispatch(updateSettings(saveFile.settings as Partial<AppSettings>))
      }
      dispatch(ActionCreators.clearHistory())
      dispatch(setStatusMessage(
        result.isLegacy
          ? `Imported legacy file: ${result.filePath}`
          : `Imported: ${result.filePath}`
      ))
    } catch (err) {
      dispatch(setStatusMessage(`Error importing file: ${err}`))
    }
  }, [dispatch])

  // ========== File Export handler ==========

  const handleFileExport = useCallback(async () => {
    if (!window.electronAPI) return
    const state = store.getState()
    const saveFile: SaveFile = {
      version: '2.0',
      settings: state.settings,
      characters: state.characters.present.list,
      stats: state.stats.definitions,
      initiative: state.initiative,
    }
    const data = JSON.stringify(saveFile, null, 2)
    const result = await window.electronAPI.fileExport(data)
    if (result) {
      dispatch(setStatusMessage(`Exported: ${result.filePath}`))
    }
  }, [dispatch])

  // ========== Clear handler ==========

  const handleClear = useCallback(async () => {
    dispatch(clearCharacters())
    dispatch(endRounds())
    dispatch(ActionCreators.clearHistory())
    if (window.electronAPI?.autosaveClear) {
      await window.electronAPI.autosaveClear()
    }
    dispatch(setStatusMessage('Session cleared'))
  }, [dispatch])

  // ========== Native menu action handler ==========

  const handleMenuAction = useCallback((action: string) => {
    const state = store.getState()
    const selectedIds = state.ui.selectedCharacterIds
    const characters = state.characters.present.list
    const initiative = state.initiative
    const settings = state.settings
    const stats = state.stats.definitions

    switch (action) {
      // File
      case 'file:import':
        handleFileImport()
        break
      case 'file:export':
        handleFileExport()
        break
      case 'file:clear':
        handleClear()
        break

      // Edit
      case 'edit:undo':
        dispatch(ActionCreators.undo())
        break
      case 'edit:redo':
        dispatch(ActionCreators.redo())
        break
      case 'edit:add':
        dispatch(openDialog({ dialog: 'editCharacter', characterId: undefined }))
        break
      case 'edit:edit':
        if (selectedIds.length > 0) {
          dispatch(openDialog({ dialog: 'editCharacter', characterId: selectedIds[0] }))
        }
        break
      case 'edit:delete':
        selectedIds.forEach(id => dispatch(removeCharacter(id)))
        break
      case 'edit:duplicate':
        selectedIds.forEach(id => dispatch(duplicateCharacter(id)))
        break
      case 'edit:purgeDead':
        dispatch(purgeDead())
        break

      // View
      case 'view:toggleHUD':
        dispatch(updateSettings({ showHUD: !settings.showHUD }))
        break

      // Rounds
      case 'rounds:start': {
        const active = sortByInitiative(characters.filter(c => !c.deleted))
        const first = active[0]
        const startingInit = first
          ? (first.manualPos > 0 ? first.manualPos : first.position)
          : 1
        dispatch(startRounds({ startingInit }))
        break
      }
      case 'rounds:end':
        dispatch(endRounds())
        break
      case 'rounds:next': {
        const result = peekNextInit(
          characters,
          initiative.currentInit,
          settings.skipDead,
          settings.deathThreshold,
        )
        dispatch(nextTurn({
          nextInit: result.nextInit,
          roundNumber: result.roundIncrement
            ? initiative.roundNumber + 1
            : initiative.roundNumber,
        }))
        break
      }
      case 'rounds:delay':
        selectedIds.forEach(id => dispatch(setCharacterStatus({ id, status: Status.Delayed })))
        break
      case 'rounds:ready':
        selectedIds.forEach(id => dispatch(setCharacterStatus({ id, status: Status.Readied })))
        break
      case 'rounds:jumpIn':
        dispatch(openDialog({ dialog: 'editCharacter', characterId: undefined }))
        break
      case 'rounds:damage':
        dispatch(openDialog({ dialog: 'damageDialog' }))
        break
      case 'rounds:stabilize':
        selectedIds.forEach(id => dispatch(stabilizeCharacter(id)))
        break
      case 'rounds:moveUp':
      case 'rounds:moveDown':
        // TODO: implement manual position moves
        break

      // Roll
      case 'roll:initiative':
        dispatch(openDialog({ dialog: 'initRollDialog' }))
        break
      default:
        // Handle dynamic roll:stat:STATID actions
        if (action.startsWith('roll:stat:')) {
          const statId = action.replace('roll:stat:', '')
          const statDef = stats.find(s => s.id === statId)
          if (statDef) {
            characters.forEach(ch => {
              if (ch.deleted) return
              const mod = ch.stats[statId]?.mod ?? 0
              const roll = makeStatRoll(statDef.dice, statDef.faces, mod)
              dispatch(setStatValue({ charId: ch.id, statId, value: { roll } }))
            })
          }
        }
        break

      // Dialogs
      case 'preferences':
        dispatch(openDialog({ dialog: 'preferences' }))
        break
      case 'statManager':
        dispatch(openDialog({ dialog: 'statManager' }))
        break
      case 'about':
        dispatch(openDialog({ dialog: 'aboutDialog' }))
        break
    }
  }, [dispatch, handleFileImport, handleFileExport, handleClear])

  // Register native menu listener
  useEffect(() => {
    if (!window.electronAPI?.onMenuAction) return
    const cleanup = window.electronAPI.onMenuAction(handleMenuAction)
    return cleanup
  }, [handleMenuAction])

  // If this is the HUD window, render HUD view
  if (hash === '#/hud') {
    return <HUDView />
  }

  return (
    <div className="app-container">
      <Toolbar />
      <CharacterTable />
      <StatusBar />

      {/* Dialogs */}
      {openDialogType === 'editCharacter' && <EditCharacterDialog />}
      {openDialogType === 'damageDialog' && <DamageDialog />}
      {openDialogType === 'initRollDialog' && <InitRollDialog />}
      {openDialogType === 'aboutDialog' && <AboutDialog />}
      {openDialogType === 'preferences' && <PreferencesDialog />}
      {openDialogType === 'effectsEditor' && <EffectsEditorDialog />}
      {openDialogType === 'statManager' && <StatManagerDialog />}
    </div>
  )
}

// HUD View — simplified player-facing display
function HUDView() {
  const characters = useAppSelector(state => state.characters.present.list)
  const initiative = useAppSelector(state => state.initiative)
  const stats = useAppSelector(state => state.stats.definitions)
  const settings = useAppSelector(state => state.settings)

  const sortedChars = [...characters]
    .filter(c => !c.deleted)
    .sort((a, b) => {
      const posA = a.manualPos > 0 ? a.manualPos : a.position
      const posB = b.manualPos > 0 ? b.manualPos : b.position
      return posB - posA
    })

  const hudStats = stats.filter(s => s.showOnHUD && !s.deleted)

  return (
    <div className="app-container">
      <div style={{ padding: '8px 12px', background: 'var(--tw-toolbar-bg)', borderBottom: '1px solid var(--tw-border)', fontWeight: 600 }}>
        Turn Watcher — Player HUD
        {initiative.inRounds && ` | Round: ${initiative.roundNumber}`}
      </div>
      <div className="character-table-container">
        <table className="character-table">
          <thead>
            <tr>
              <th>Name</th>
              {hudStats.map(s => (
                <th key={s.id}>{s.name}</th>
              ))}
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            {sortedChars.map(ch => {
              const pos = ch.manualPos > 0 ? ch.manualPos : ch.position
              const isCurrent = initiative.inRounds && pos === initiative.currentInit
              const displayName = ch.publicName || ch.name

              return (
                <tr key={ch.id} className={isCurrent ? 'current-init' : ''}>
                  <td>
                    <span className="character-name">
                      {isCurrent && <span className="init-arrow">→</span>}
                      {displayName}
                    </span>
                  </td>
                  {hudStats.map(s => {
                    if (ch.monster && !s.showMonsterOnHUD) {
                      return <td key={s.id}>—</td>
                    }
                    const val = ch.stats[s.id]
                    return <td key={s.id}>{val?.roll ?? '—'}</td>
                  })}
                  <td>{getStatusString(ch, settings.deathThreshold)}</td>
                </tr>
              )
            })}
          </tbody>
        </table>
      </div>
      <div className="status-bar">
        <div className="status-bar-section">
          {initiative.inRounds ? `Round: ${initiative.roundNumber}` : 'Not in rounds'}
        </div>
      </div>
    </div>
  )
}
