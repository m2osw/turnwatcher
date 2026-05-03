import { useCallback } from 'react'
import { useAppSelector, useAppDispatch } from './store'
import { ActionCreators } from 'redux-undo'
import { setCharacters } from './store/slices/charactersSlice'
import { setInitiativeState } from './store/slices/initiativeSlice'
import { setStats } from './store/slices/statsSlice'
import { updateSettings } from './store/slices/settingsSlice'
import { setCurrentFilePath, setModified, setStatusMessage } from './store/slices/uiSlice'
import { MenuBar } from './components/MainWindow/MenuBar'
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
import { SaveFile, AppSettings } from './types'

export default function App() {
  const dispatch = useAppDispatch()
  const openDialog = useAppSelector(state => state.ui.openDialog)
  const hash = window.location.hash

  // Handle file open
  const handleFileOpen = useCallback(async () => {
    if (!window.electronAPI) return
    const result = await window.electronAPI.fileOpen()
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
      dispatch(setCurrentFilePath(result.filePath))
      dispatch(setModified(false))
      dispatch(ActionCreators.clearHistory())
      dispatch(setStatusMessage(
        result.isLegacy
          ? `Imported legacy file: ${result.filePath}`
          : `Opened: ${result.filePath}`
      ))
    } catch (err) {
      dispatch(setStatusMessage(`Error loading file: ${err}`))
    }
  }, [dispatch])

  // Handle file save
  const handleFileSave = useCallback(async (saveAs: boolean = false) => {
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
    const filePath = saveAs ? undefined : state.ui.currentFilePath || undefined
    const result = await window.electronAPI.fileSave(data, filePath)
    if (result) {
      dispatch(setCurrentFilePath(result.filePath))
      dispatch(setModified(false))
      dispatch(setStatusMessage(`Saved: ${result.filePath}`))
    }
  }, [dispatch])

  // If this is the HUD window, render HUD view
  if (hash === '#/hud') {
    return <HUDView />
  }

  return (
    <div className="app-container">
      <MenuBar onFileOpen={handleFileOpen} onFileSave={handleFileSave} />
      <Toolbar />
      <CharacterTable />
      <StatusBar />

      {/* Dialogs */}
      {openDialog === 'editCharacter' && <EditCharacterDialog />}
      {openDialog === 'damageDialog' && <DamageDialog />}
      {openDialog === 'initRollDialog' && <InitRollDialog />}
      {openDialog === 'aboutDialog' && <AboutDialog />}
      {openDialog === 'preferences' && <PreferencesDialog />}
      {openDialog === 'effectsEditor' && <EffectsEditorDialog />}
      {openDialog === 'statManager' && <StatManagerDialog />}
    </div>
  )
}

// Need to import store for save
import { store } from './store'

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

              // For monsters, only show stats marked showMonsterOnHUD
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
                  <td>
                    {(() => {
                      const { getStatusString } = require('./utils/health')
                      return getStatusString(ch, settings.deathThreshold)
                    })()}
                  </td>
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
