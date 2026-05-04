import { useAppDispatch, useAppSelector } from '../../store'
import { setSelectedCharacters, openDialog } from '../../store/slices/uiSlice'
import { Status, Health, Character } from '../../types'
import { getHealth, getHPDisplay, getStatusString } from '../../utils/health'
import { sortByInitiative } from '../../utils/initiative'

export function CharacterTable() {
  const dispatch = useAppDispatch()
  const characters = useAppSelector(state => state.characters.present.list)
  const initiative = useAppSelector(state => state.initiative)
  const selectedIds = useAppSelector(state => state.ui.selectedCharacterIds)
  const settings = useAppSelector(state => state.settings)
  const stats = useAppSelector(state => state.stats.definitions)

  // Visible columns: stats that appear on the table (toolbar stats + HP)
  const visibleStats = stats
    .filter(s => !s.deleted && !s.ability && s.id !== 'HITPOINTS_ID' && s.id !== 'LEVEL_ID')
    .sort((a, b) => a.order - b.order)

  // Sort characters by initiative if in rounds, otherwise by insertion order
  const sortedChars = initiative.inRounds
    ? sortByInitiative(characters.filter(c => !c.deleted))
    : characters.filter(c => !c.deleted)

  const handleRowClick = (ch: Character, e: React.MouseEvent) => {
    if (e.ctrlKey || e.metaKey) {
      // Toggle selection
      const newSel = selectedIds.includes(ch.id)
        ? selectedIds.filter(id => id !== ch.id)
        : [...selectedIds, ch.id]
      dispatch(setSelectedCharacters(newSel))
    } else {
      dispatch(setSelectedCharacters([ch.id]))
    }
  }

  const handleRowDoubleClick = (ch: Character) => {
    dispatch(openDialog({ dialog: 'editCharacter', characterId: ch.id }))
  }

  const getRowClasses = (ch: Character): string => {
    const classes: string[] = []
    const pos = ch.manualPos > 0 ? ch.manualPos : ch.position
    const health = getHealth(ch, settings.deathThreshold)

    if (selectedIds.includes(ch.id)) classes.push('selected')
    if (initiative.inRounds && pos === initiative.currentInit) classes.push('current-init')
    if (ch.monster) classes.push('monster-row')
    if (ch.status === Status.Delayed) classes.push('delayed-row')
    if (ch.status === Status.Readied) classes.push('readied-row')
    if (health === Health.Dead) classes.push('dead-row')
    if (health === Health.Dying) classes.push('dying-row')

    return classes.join(' ')
  }

  return (
    <div className="character-table-container">
      <table className="character-table">
        <thead>
          <tr>
            <th>Name</th>
            <th style={{ width: 40 }}>Type</th>
            {visibleStats.map(s => (
              <th key={s.id} style={{ width: 60 }}>{s.name.substring(0, 6)}</th>
            ))}
            <th style={{ width: 80 }}>Hit Points</th>
            <th style={{ width: 80 }}>Status</th>
          </tr>
        </thead>
        <tbody>
          {sortedChars.map(ch => {
            const pos = ch.manualPos > 0 ? ch.manualPos : ch.position
            const isCurrent = initiative.inRounds && pos === initiative.currentInit

            return (
              <tr
                key={ch.id}
                className={getRowClasses(ch)}
                onClick={(e) => handleRowClick(ch, e)}
                onDoubleClick={() => handleRowDoubleClick(ch)}
              >
                <td>
                  <span className="character-name">
                    {isCurrent && <span className="init-arrow">→</span>}
                    <img
                      className="type-icon"
                      src={ch.monster ? '/assets/monster_icon.png' : '/assets/hero.png'}
                      alt={ch.monster ? 'Monster' : 'PC'}
                    />
                    {ch.name}
                  </span>
                </td>
                <td>
                  <img
                    style={{ width: 16, height: 16 }}
                    src={ch.monster ? '/assets/monster_icon.png' : '/assets/hero.png'}
                    alt={ch.monster ? 'M' : 'PC'}
                  />
                </td>
                {visibleStats.map(s => {
                  const val = ch.stats[s.id]
                  return (
                    <td key={s.id}>
                      {val?.roll !== undefined && val.roll !== 0 ? val.roll : '—'}
                    </td>
                  )
                })}
                <td>{getHPDisplay(ch)}</td>
                <td>{getStatusString(ch, settings.deathThreshold)}</td>
              </tr>
            )
          })}
          {sortedChars.length === 0 && (
            <tr>
              <td
                colSpan={visibleStats.length + 4}
                style={{ textAlign: 'center', padding: 40, color: 'var(--tw-text-muted)' }}
              >
                No characters. Click "Add" to create one, or open an existing file.
              </td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  )
}
