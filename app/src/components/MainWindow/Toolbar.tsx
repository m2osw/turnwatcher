import { useAppDispatch, useAppSelector } from '../../store'
import { removeCharacter } from '../../store/slices/charactersSlice'
import { endRounds, nextTurn } from '../../store/slices/initiativeSlice'
import { openDialog } from '../../store/slices/uiSlice'
import { setCharacterStatus, resetAllStatuses } from '../../store/slices/charactersSlice'
import { Status } from '../../types'
import { peekNextInit, buildStartRoundsResult, getCharacterAtPosition } from '../../utils/initiative'
import { makeStatRoll } from '../../utils/dice'
import { setStatValue } from '../../store/slices/charactersSlice'

export function Toolbar() {
  const dispatch = useAppDispatch()
  const selectedIds = useAppSelector(state => state.ui.selectedCharacterIds)
  const characters = useAppSelector(state => state.characters.present.list)
  const initiative = useAppSelector(state => state.initiative)
  const settings = useAppSelector(state => state.settings)
  const stats = useAppSelector(state => state.stats.definitions)

  const hasSelection = selectedIds.length > 0
  const hasCharacters = characters.length > 0

  const handleDelete = () => {
    selectedIds.forEach(id => dispatch(removeCharacter(id)))
  }

  const handleStart = () => {
    const result = buildStartRoundsResult(characters, stats, settings)
    if (result.openDialog) {
      // Manual initiative mode: dispatch NPC rolls, then open the dialog
      // so the DM can enter player rolls before starting rounds.
      result.actionsToDispatch.forEach(a => dispatch(a as Parameters<typeof dispatch>[0]))
      dispatch(openDialog({ dialog: 'initRollDialog' }))
    } else {
      result.actionsToDispatch.forEach(a => dispatch(a as Parameters<typeof dispatch>[0]))
    }
  }

  const handleNext = () => {
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
  }

  const handleDelay = () => {
    // Apply Delay to the current-initiative character, then advance the turn
    const currentChar = getCharacterAtPosition(
      characters.filter(c => !c.deleted),
      initiative.currentInit,
    )
    if (currentChar) {
      dispatch(setCharacterStatus({ id: currentChar.id, status: Status.Delayed }))
      handleNext()
    }
  }

  const handleReady = () => {
    // Apply Ready to the current-initiative character, then advance the turn
    const currentChar = getCharacterAtPosition(
      characters.filter(c => !c.deleted),
      initiative.currentInit,
    )
    if (currentChar) {
      dispatch(setCharacterStatus({ id: currentChar.id, status: Status.Readied }))
      handleNext()
    }
  }

  const handleEnd = () => {
    dispatch(resetAllStatuses())
    dispatch(endRounds())
  }

  const handleRollStat = (statId: string) => {
    const statDef = stats.find(s => s.id === statId)
    if (!statDef) return
    characters.forEach(ch => {
      if (ch.deleted) return
      const mod = ch.stats[statId]?.mod ?? 0
      const roll = makeStatRoll(statDef.dice, statDef.faces, mod)
      dispatch(setStatValue({ charId: ch.id, statId, value: { roll } }))
    })
  }

  // Toolbar stats that show on toolbar
  const toolbarStats = stats.filter(s => s.showOnToolbar && !s.deleted)

  // Map stat IDs to icon filenames
  const statIcons: Record<string, string> = {
    'INIT_ID': 'd20_init.png',
    'SPOT_ID': 'd20_spot.png',
    'LISTEN_ID': 'd20_listen.png',
    'WILL_ID': 'd20_will.png',
  }

  return (
    <div className="toolbar">
      <button
        className="toolbar-button"
        onClick={() => dispatch(openDialog({ dialog: 'editCharacter', characterId: undefined }))}
        title="Add Character"
      >
        <img src="/assets/add.png" alt="Add" />
        <span>Add</span>
      </button>

      <button
        className="toolbar-button"
        disabled={!hasSelection}
        onClick={() => {
          if (selectedIds.length > 0) {
            dispatch(openDialog({ dialog: 'editCharacter', characterId: selectedIds[0] }))
          }
        }}
        title="Edit Character"
      >
        <img src="/assets/edit.png" alt="Edit" />
        <span>Edit</span>
      </button>

      <button
        className="toolbar-button"
        disabled={!hasSelection}
        onClick={handleDelete}
        title="Delete Character"
      >
        <img src="/assets/delete.png" alt="Delete" />
        <span>Delete</span>
      </button>

      <div className="toolbar-separator" />

      {/* Start / End toggle */}
      {!initiative.inRounds ? (
        <button
          className="toolbar-button"
          disabled={!hasCharacters}
          onClick={handleStart}
          title="Start Rounds"
        >
          <img src="/assets/start.png" alt="Start" />
          <span>Start</span>
        </button>
      ) : (
        <button
          className="toolbar-button"
          onClick={handleEnd}
          title="End Rounds"
        >
          <img src="/assets/end.png" alt="End" />
          <span>End</span>
        </button>
      )}

      <button
        className="toolbar-button"
        disabled={!initiative.inRounds || characters.filter(c => !c.deleted).length === 0}
        onClick={handleNext}
        title="Next Turn"
      >
        <img src="/assets/next.png" alt="Next" />
        <span>Next</span>
      </button>

      <button
        className="toolbar-button"
        disabled={!initiative.inRounds}
        onClick={handleDelay}
        title="Delay Action"
      >
        <img src="/assets/delay.png" alt="Delay" />
        <span>Delay</span>
      </button>

      <button
        className="toolbar-button"
        disabled={!initiative.inRounds}
        onClick={handleReady}
        title="Ready Action"
      >
        <img src="/assets/ready.png" alt="Ready" />
        <span>Ready</span>
      </button>

      <button
        className="toolbar-button"
        disabled={!initiative.inRounds}
        onClick={() => dispatch(openDialog({ dialog: 'jumpIn' }))}
        title="Jump In"
      >
        <img src="/assets/jump_in.png" alt="Jump In" />
        <span>Jump In</span>
      </button>

      <button
        className="toolbar-button"
        disabled={!hasSelection}
        onClick={() => dispatch(openDialog({ dialog: 'damageDialog' }))}
        title="Apply Damage"
      >
        <img src="/assets/damage.png" alt="Damage" />
        <span>Damage</span>
      </button>

      <div className="toolbar-separator" />

      {/* Roll stat buttons */}
      {toolbarStats.map(stat => (
        <button
          key={stat.id}
          className="toolbar-button"
          disabled={!hasCharacters}
          onClick={() => handleRollStat(stat.id)}
          title={`Roll ${stat.name}`}
        >
          <img
            src={`/assets/${statIcons[stat.id] || 'd20icon1.png'}`}
            alt={stat.name}
          />
          <span>{stat.name}</span>
        </button>
      ))}
    </div>
  )
}
