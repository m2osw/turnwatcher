import { useState, useEffect } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { addCharacter, updateCharacter } from '../../store/slices/charactersSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { Character, Status, StatValue } from '../../types'
import { v4 as uuidv4 } from 'uuid'

export function EditCharacterDialog() {
  const dispatch = useAppDispatch()
  const editingId = useAppSelector(state => state.ui.editingCharacterId)
  const characters = useAppSelector(state => state.characters.present.list)
  const statDefs = useAppSelector(state => state.stats.definitions)
  const isNew = !editingId

  const existingChar = editingId ? characters.find(c => c.id === editingId) : null

  const [name, setName] = useState('')
  const [publicName, setPublicName] = useState('')
  const [notes, setNotes] = useState('')
  const [monster, setMonster] = useState(false)
  const [baseHP, setBaseHP] = useState(10)
  const [hitDice, setHitDice] = useState(1)
  const [statValues, setStatValues] = useState<Record<string, StatValue>>({})
  const [activeTab, setActiveTab] = useState('base')

  useEffect(() => {
    if (existingChar) {
      setName(existingChar.name)
      setPublicName(existingChar.publicName)
      setNotes(existingChar.notes)
      setMonster(existingChar.monster)
      setBaseHP(existingChar.baseHP)
      setHitDice(existingChar.hitDice)
      setStatValues({ ...existingChar.stats })
    }
  }, [existingChar])

  const handleSave = () => {
    if (isNew) {
      dispatch(addCharacter({
        id: uuidv4(),
        name,
        publicName,
        notes,
        monster,
        baseHP,
        hitDice,
        stats: statValues,
      }))
    } else {
      dispatch(updateCharacter({
        id: editingId!,
        changes: { name, publicName, notes, monster, baseHP, hitDice, stats: statValues },
      }))
    }
    dispatch(closeDialog())
  }

  const updateStatMod = (statId: string, mod: number) => {
    setStatValues(prev => ({
      ...prev,
      [statId]: { ...(prev[statId] || { mod: 0, roll: 0, notes: '' }), mod },
    }))
  }

  const abilityStats = statDefs.filter(s => s.ability && !s.deleted).sort((a, b) => a.order - b.order)
  const skillStats = statDefs.filter(s => !s.ability && !s.internal && !s.deleted).sort((a, b) => a.order - b.order)

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 550 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>{isNew ? 'Add Character' : `Edit: ${existingChar?.name}`}</span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>

        <div className="tabs">
          <button className={`tab ${activeTab === 'base' ? 'active' : ''}`} onClick={() => setActiveTab('base')}>Base Info</button>
          <button className={`tab ${activeTab === 'abilities' ? 'active' : ''}`} onClick={() => setActiveTab('abilities')}>Abilities</button>
          <button className={`tab ${activeTab === 'skills' ? 'active' : ''}`} onClick={() => setActiveTab('skills')}>Skills & Saves</button>
          <button className={`tab ${activeTab === 'notes' ? 'active' : ''}`} onClick={() => setActiveTab('notes')}>Notes</button>
        </div>

        <div className="modal-body">
          {activeTab === 'base' && (
            <>
              <div className="form-group">
                <label>Character Name</label>
                <input type="text" value={name} onChange={e => setName(e.target.value)} autoFocus />
              </div>
              <div className="form-group">
                <label>Public Name (shown on HUD)</label>
                <input type="text" value={publicName} onChange={e => setPublicName(e.target.value)} />
              </div>
              <div className="form-row">
                <div className="form-group">
                  <label>Base Hit Points</label>
                  <input type="number" value={baseHP} onChange={e => setBaseHP(parseInt(e.target.value) || 0)} />
                </div>
                <div className="form-group">
                  <label>Hit Dice</label>
                  <input type="number" value={hitDice} onChange={e => setHitDice(parseInt(e.target.value) || 1)} min={1} />
                </div>
              </div>
              <div className="form-group">
                <div className="checkbox-group">
                  <input type="checkbox" id="monster" checked={monster} onChange={e => setMonster(e.target.checked)} />
                  <label htmlFor="monster">Monster / NPC</label>
                </div>
              </div>
              <div className="form-group">
                <label>Initiative Modifier</label>
                <input
                  type="number"
                  value={statValues['INIT_ID']?.mod ?? 0}
                  onChange={e => updateStatMod('INIT_ID', parseInt(e.target.value) || 0)}
                />
              </div>
            </>
          )}

          {activeTab === 'abilities' && (
            <>
              {abilityStats.map(stat => (
                <div className="form-row" key={stat.id}>
                  <div className="form-group">
                    <label>{stat.name} Score</label>
                    <input
                      type="number"
                      value={statValues[stat.id]?.roll ?? 0}
                      onChange={e => setStatValues(prev => ({
                        ...prev,
                        [stat.id]: { ...(prev[stat.id] || { mod: 0, roll: 0, notes: '' }), roll: parseInt(e.target.value) || 0 },
                      }))}
                    />
                  </div>
                  <div className="form-group">
                    <label>{stat.name} Modifier</label>
                    <input
                      type="number"
                      value={statValues[stat.id]?.mod ?? 0}
                      onChange={e => updateStatMod(stat.id, parseInt(e.target.value) || 0)}
                    />
                  </div>
                </div>
              ))}
            </>
          )}

          {activeTab === 'skills' && (
            <>
              {skillStats.map(stat => (
                <div className="form-row" key={stat.id}>
                  <div className="form-group" style={{ flex: 2 }}>
                    <label>{stat.name}</label>
                  </div>
                  <div className="form-group">
                    <label>Modifier</label>
                    <input
                      type="number"
                      value={statValues[stat.id]?.mod ?? 0}
                      onChange={e => updateStatMod(stat.id, parseInt(e.target.value) || 0)}
                    />
                  </div>
                </div>
              ))}
            </>
          )}

          {activeTab === 'notes' && (
            <div className="form-group">
              <label>Notes</label>
              <textarea
                rows={10}
                value={notes}
                onChange={e => setNotes(e.target.value)}
                style={{ resize: 'vertical' }}
              />
            </div>
          )}
        </div>

        <div className="modal-footer">
          <button className="btn" onClick={() => dispatch(closeDialog())}>Cancel</button>
          <button className="btn btn-primary" onClick={handleSave} disabled={!name.trim()}>
            {isNew ? 'Add' : 'Save'}
          </button>
        </div>
      </div>
    </div>
  )
}
