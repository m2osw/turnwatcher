import { useAppSelector } from '../../store'
import { getCharacterAtPosition } from '../../utils/initiative'

export function StatusBar() {
  const initiative = useAppSelector(state => state.initiative)
  const characters = useAppSelector(state => state.characters.present.list)
  const statusMessage = useAppSelector(state => state.ui.statusMessage)
  const selectedIds = useAppSelector(state => state.ui.selectedCharacterIds)

  // Find current character in initiative
  let currentCharName = ''
  if (initiative.inRounds) {
    const ch = getCharacterAtPosition(characters, initiative.currentInit)
    if (ch) currentCharName = ch.name
  }

  // Find selected character's status
  let selectionStatus = ''
  if (selectedIds.length === 1) {
    const ch = characters.find(c => c.id === selectedIds[0])
    if (ch) selectionStatus = ch.name
  } else if (selectedIds.length > 1) {
    selectionStatus = `${selectedIds.length} selected`
  }

  return (
    <div className="status-bar">
      <div className="status-bar-section" style={{ minWidth: 200 }}>
        {initiative.inRounds
          ? `Next In Initiative Order${currentCharName ? `: ${currentCharName}` : ''}`
          : statusMessage || 'Ready'
        }
      </div>
      <div className="status-bar-section" style={{ minWidth: 100 }}>
        {selectionStatus || 'Normal'}
      </div>
      <div className="status-bar-section">
        {initiative.inRounds
          ? `Round: ${initiative.roundNumber}`
          : ''
        }
      </div>
    </div>
  )
}
