import { useState } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { addEffect, updateEffect, removeEffect } from '../../store/slices/charactersSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { Effect, EffectType } from '../../types'
import { v4 as uuidv4 } from 'uuid'

export function EffectsEditorDialog() {
  const dispatch = useAppDispatch()
  const editingId = useAppSelector(state => state.ui.editingCharacterId)
  const characters = useAppSelector(state => state.characters.present.list)
  const character = characters.find(c => c.id === editingId)

  const [editingEffect, setEditingEffect] = useState<Effect | null>(null)
  const [isAdding, setIsAdding] = useState(false)

  if (!character) {
    return (
      <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
        <div className="modal" onClick={e => e.stopPropagation()}>
          <div className="modal-header">
            <span>Effects Editor</span>
            <button onClick={() => dispatch(closeDialog())}>×</button>
          </div>
          <div className="modal-body">
            <p>No character selected.</p>
          </div>
        </div>
      </div>
    )
  }

  const handleAdd = () => {
    setEditingEffect({
      id: uuidv4(),
      name: '',
      description: '',
      type: EffectType.Spell,
      totalRounds: 10,
      roundsUsed: 0,
      startIn: 0,
      tempHP: 0,
      hpBoost: 0,
      isActive: true,
    })
    setIsAdding(true)
  }

  const handleSaveEffect = () => {
    if (!editingEffect || !editingEffect.name.trim()) return
    if (isAdding) {
      dispatch(addEffect({ charId: character.id, effect: editingEffect }))
    } else {
      dispatch(updateEffect({
        charId: character.id,
        effectId: editingEffect.id,
        changes: editingEffect,
      }))
    }
    setEditingEffect(null)
    setIsAdding(false)
  }

  const handleDelete = (effectId: string) => {
    dispatch(removeEffect({ charId: character.id, effectId }))
  }

  if (editingEffect) {
    return (
      <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
        <div className="modal" style={{ width: 450 }} onClick={e => e.stopPropagation()}>
          <div className="modal-header">
            <span>{isAdding ? 'Add Effect' : 'Edit Effect'}</span>
            <button onClick={() => { setEditingEffect(null); setIsAdding(false) }}>×</button>
          </div>
          <div className="modal-body">
            <div className="form-group">
              <label>Effect Name</label>
              <input type="text" value={editingEffect.name} onChange={e => setEditingEffect({ ...editingEffect, name: e.target.value })} autoFocus />
            </div>
            <div className="form-group">
              <label>Description</label>
              <textarea rows={3} value={editingEffect.description} onChange={e => setEditingEffect({ ...editingEffect, description: e.target.value })} />
            </div>
            <div className="form-group">
              <label>Type</label>
              <select value={editingEffect.type} onChange={e => setEditingEffect({ ...editingEffect, type: parseInt(e.target.value) })}>
                <option value={EffectType.Spell}>Spell</option>
                <option value={EffectType.Ability}>Ability</option>
                <option value={EffectType.Other}>Other</option>
              </select>
            </div>
            <div className="form-row">
              <div className="form-group">
                <label>Total Rounds</label>
                <input type="number" value={editingEffect.totalRounds} onChange={e => setEditingEffect({ ...editingEffect, totalRounds: parseInt(e.target.value) || 0 })} />
              </div>
              <div className="form-group">
                <label>Start In (rounds)</label>
                <input type="number" value={editingEffect.startIn} onChange={e => setEditingEffect({ ...editingEffect, startIn: parseInt(e.target.value) || 0 })} />
              </div>
            </div>
            <div className="form-row">
              <div className="form-group">
                <label>HP Boost</label>
                <input type="number" value={editingEffect.hpBoost} onChange={e => setEditingEffect({ ...editingEffect, hpBoost: parseInt(e.target.value) || 0 })} />
              </div>
              <div className="form-group">
                <label>Temp HP</label>
                <input type="number" value={editingEffect.tempHP} onChange={e => setEditingEffect({ ...editingEffect, tempHP: parseInt(e.target.value) || 0 })} />
              </div>
            </div>
          </div>
          <div className="modal-footer">
            <button className="btn" onClick={() => { setEditingEffect(null); setIsAdding(false) }}>Cancel</button>
            <button className="btn btn-primary" onClick={handleSaveEffect} disabled={!editingEffect.name.trim()}>Save</button>
          </div>
        </div>
      </div>
    )
  }

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 500 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>Effects: {character.name}</span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>
        <div className="modal-body">
          {character.effects.length === 0 ? (
            <div style={{ color: 'var(--tw-text-muted)', textAlign: 'center', padding: 20 }}>
              No active effects.
            </div>
          ) : (
            <table style={{ width: '100%', borderCollapse: 'collapse' }}>
              <thead>
                <tr>
                  <th style={{ textAlign: 'left', padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Name</th>
                  <th style={{ padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Type</th>
                  <th style={{ padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Rounds Left</th>
                  <th style={{ padding: '4px 8px', borderBottom: '1px solid var(--tw-border)' }}>Actions</th>
                </tr>
              </thead>
              <tbody>
                {character.effects.map(eff => (
                  <tr key={eff.id}>
                    <td style={{ padding: '4px 8px' }}>{eff.name}</td>
                    <td style={{ textAlign: 'center', padding: '4px 8px' }}>
                      {eff.type === EffectType.Spell ? 'Spell' : eff.type === EffectType.Ability ? 'Ability' : 'Other'}
                    </td>
                    <td style={{ textAlign: 'center', padding: '4px 8px' }}>
                      {eff.totalRounds - eff.roundsUsed}
                    </td>
                    <td style={{ textAlign: 'center', padding: '4px 8px' }}>
                      <button className="btn" style={{ padding: '2px 8px', fontSize: 11, marginRight: 4 }} onClick={() => { setEditingEffect({ ...eff }); setIsAdding(false) }}>Edit</button>
                      <button className="btn btn-danger" style={{ padding: '2px 8px', fontSize: 11 }} onClick={() => handleDelete(eff.id)}>×</button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
        <div className="modal-footer">
          <button className="btn" onClick={handleAdd}>Add Effect</button>
          <button className="btn" onClick={() => dispatch(closeDialog())}>Close</button>
        </div>
      </div>
    </div>
  )
}
