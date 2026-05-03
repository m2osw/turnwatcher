import { useState } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { applyDamage, healCharacter, fullHeal } from '../../store/slices/charactersSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { getHPDisplay } from '../../utils/health'

export function DamageDialog() {
  const dispatch = useAppDispatch()
  const selectedIds = useAppSelector(state => state.ui.selectedCharacterIds)
  const characters = useAppSelector(state => state.characters.present.list)
  const [value, setValue] = useState(0)

  const selectedChars = characters.filter(c => selectedIds.includes(c.id))

  const handleDamage = () => {
    selectedIds.forEach(id => {
      dispatch(applyDamage({ id, amount: value }))
    })
    dispatch(closeDialog())
  }

  const handleHeal = () => {
    selectedIds.forEach(id => {
      dispatch(healCharacter({ id, amount: value }))
    })
    dispatch(closeDialog())
  }

  const handleFullHeal = () => {
    selectedIds.forEach(id => {
      dispatch(fullHeal(id))
    })
    dispatch(closeDialog())
  }

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 400 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>
            {selectedChars.length === 1
              ? `Damage / Heal: ${selectedChars[0].name}`
              : `Damage / Heal: ${selectedChars.length} characters`
            }
          </span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>

        <div className="modal-body">
          {selectedChars.length === 1 && (
            <div style={{ marginBottom: 12, color: 'var(--tw-text-muted)' }}>
              Current HP: {getHPDisplay(selectedChars[0])}
            </div>
          )}

          <div className="form-group">
            <label>Amount</label>
            <input
              type="number"
              value={value}
              onChange={e => setValue(parseInt(e.target.value) || 0)}
              autoFocus
              min={0}
            />
          </div>
        </div>

        <div className="modal-footer">
          <button className="btn" onClick={handleFullHeal}>Full Heal</button>
          <button className="btn" onClick={() => dispatch(closeDialog())}>Cancel</button>
          <button className="btn btn-primary" onClick={handleHeal} disabled={value <= 0}>Heal</button>
          <button className="btn btn-danger" onClick={handleDamage} disabled={value <= 0}>Damage</button>
        </div>
      </div>
    </div>
  )
}
