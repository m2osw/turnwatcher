import { useState, useEffect } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { setStatValue } from '../../store/slices/charactersSlice'
import { startRounds } from '../../store/slices/initiativeSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { makeStatRoll } from '../../utils/dice'
import { Character } from '../../types'

interface CharRoll {
  id: string
  name: string
  mod: number
  roll: number
  monster: boolean
}

export function InitRollDialog() {
  const dispatch = useAppDispatch()
  const characters = useAppSelector(state => state.characters.present.list)
  const stats = useAppSelector(state => state.stats.definitions)
  const settings = useAppSelector(state => state.settings)

  const initStat = stats.find(s => s.id === 'INIT_ID')

  const [rolls, setRolls] = useState<CharRoll[]>([])

  useEffect(() => {
    const initial = characters
      .filter(c => !c.deleted)
      .map(ch => ({
        id: ch.id,
        name: ch.name,
        mod: ch.stats['INIT_ID']?.mod ?? 0,
        roll: settings.manualInitiative
          ? 0
          : makeStatRoll(initStat?.dice ?? 1, initStat?.faces ?? 20, ch.stats['INIT_ID']?.mod ?? 0),
        monster: ch.monster,
      }))
    setRolls(initial)
  }, [characters, initStat, settings.manualInitiative])

  const handleRollChange = (id: string, value: number) => {
    setRolls(prev => prev.map(r => r.id === id ? { ...r, roll: value } : r))
  }

  const handleReroll = () => {
    setRolls(prev => prev.map(r => ({
      ...r,
      roll: makeStatRoll(initStat?.dice ?? 1, initStat?.faces ?? 20, r.mod),
    })))
  }

  const handleAccept = () => {
    // Apply rolls to characters
    rolls.forEach(r => {
      dispatch(setStatValue({ charId: r.id, statId: 'INIT_ID', value: { roll: r.roll } }))
    })
    // Start rounds
    dispatch(startRounds())
    dispatch(closeDialog())
  }

  const pcs = rolls.filter(r => !r.monster)
  const monsters = rolls.filter(r => r.monster)

  const renderTable = (title: string, charRolls: CharRoll[]) => (
    <div style={{ marginBottom: 16 }}>
      <div style={{ fontWeight: 600, marginBottom: 4 }}>{title}</div>
      <table style={{ width: '100%', borderCollapse: 'collapse' }}>
        <thead>
          <tr>
            <th style={{ textAlign: 'left', padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Name</th>
            <th style={{ width: 60, padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Mod</th>
            <th style={{ width: 80, padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Roll</th>
          </tr>
        </thead>
        <tbody>
          {charRolls.map(r => (
            <tr key={r.id}>
              <td style={{ padding: '4px 8px' }}>{r.name}</td>
              <td style={{ textAlign: 'center', padding: '4px 8px' }}>{r.mod >= 0 ? `+${r.mod}` : r.mod}</td>
              <td style={{ padding: '4px 8px' }}>
                <input
                  type="number"
                  value={r.roll}
                  onChange={e => handleRollChange(r.id, parseInt(e.target.value) || 0)}
                  style={{ width: '100%', textAlign: 'center', padding: '2px 4px' }}
                />
              </td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  )

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 450 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>Roll Initiative</span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>

        <div className="modal-body" style={{ maxHeight: 400 }}>
          {pcs.length > 0 && renderTable('Player Characters', pcs)}
          {monsters.length > 0 && renderTable('Monsters / NPCs', monsters)}
          {rolls.length === 0 && (
            <div style={{ color: 'var(--tw-text-muted)', textAlign: 'center', padding: 20 }}>
              No characters to roll for.
            </div>
          )}
        </div>

        <div className="modal-footer">
          <button className="btn" onClick={handleReroll}>Re-Roll</button>
          <button className="btn" onClick={() => dispatch(closeDialog())}>Cancel</button>
          <button className="btn btn-primary" onClick={handleAccept} disabled={rolls.length === 0}>
            Accept & Start
          </button>
        </div>
      </div>
    </div>
  )
}
