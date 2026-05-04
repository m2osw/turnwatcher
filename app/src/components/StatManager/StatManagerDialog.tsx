import { useState } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { updateStat } from '../../store/slices/statsSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { StatDefinition } from '../../types'

export function StatManagerDialog() {
  const dispatch = useAppDispatch()
  const statDefs = useAppSelector(state => state.stats.definitions)
  const [selectedStatId, setSelectedStatId] = useState<string | null>(null)

  const visibleStats = statDefs.filter(s => !s.deleted).sort((a, b) => a.order - b.order)
  const selectedStat = statDefs.find(s => s.id === selectedStatId)

  const handleUpdate = (changes: Partial<StatDefinition>) => {
    if (selectedStatId) {
      dispatch(updateStat({ id: selectedStatId, changes }))
    }
  }

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 600 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>Stat Manager</span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>
        <div className="modal-body" style={{ display: 'flex', gap: 16 }}>
          {/* Stat list */}
          <div style={{ width: 200, borderRight: '1px solid var(--tw-border)', paddingRight: 16 }}>
            {visibleStats.map(s => (
              <div
                key={s.id}
                onClick={() => setSelectedStatId(s.id)}
                style={{
                  padding: '6px 8px',
                  cursor: 'pointer',
                  background: selectedStatId === s.id ? 'var(--tw-accent)' : 'transparent',
                  color: selectedStatId === s.id ? 'white' : 'var(--tw-text)',
                  borderRadius: 3,
                  marginBottom: 2,
                  fontSize: 13,
                }}
              >
                {s.name}
                {s.internal && <span style={{ fontSize: 10, marginLeft: 4, opacity: 0.7 }}>(internal)</span>}
              </div>
            ))}
          </div>

          {/* Stat editor */}
          <div style={{ flex: 1 }}>
            {selectedStat ? (
              <>
                <div className="form-group">
                  <label>Name</label>
                  <input type="text" value={selectedStat.name} onChange={e => handleUpdate({ name: e.target.value })} disabled={selectedStat.internal} />
                </div>
                <div className="form-row">
                  <div className="form-group">
                    <label>Dice Count</label>
                    <input type="number" value={selectedStat.dice} onChange={e => handleUpdate({ dice: parseInt(e.target.value) || 0 })} />
                  </div>
                  <div className="form-group">
                    <label>Dice Faces</label>
                    <input type="number" value={selectedStat.faces} onChange={e => handleUpdate({ faces: parseInt(e.target.value) || 0 })} />
                  </div>
                </div>
                <div className="form-group">
                  <div className="checkbox-group">
                    <input type="checkbox" checked={selectedStat.showOnToolbar} onChange={e => handleUpdate({ showOnToolbar: e.target.checked })} />
                    <label>Show on Toolbar</label>
                  </div>
                </div>
                <div className="form-group">
                  <div className="checkbox-group">
                    <input type="checkbox" checked={selectedStat.showOnHUD} onChange={e => handleUpdate({ showOnHUD: e.target.checked })} />
                    <label>Show on Player HUD</label>
                  </div>
                </div>
                <div className="form-group">
                  <div className="checkbox-group">
                    <input type="checkbox" checked={selectedStat.showMonsterOnHUD} onChange={e => handleUpdate({ showMonsterOnHUD: e.target.checked })} />
                    <label>Show Monster Stats on HUD</label>
                  </div>
                </div>
                <div className="form-group">
                  <label>Accelerator Key</label>
                  <input type="text" value={selectedStat.accel} onChange={e => handleUpdate({ accel: e.target.value })} maxLength={1} style={{ width: 40 }} />
                </div>
              </>
            ) : (
              <div style={{ color: 'var(--tw-text-muted)', textAlign: 'center', padding: 40 }}>
                Select a stat to edit its properties.
              </div>
            )}
          </div>
        </div>
        <div className="modal-footer">
          <button className="btn btn-primary" onClick={() => dispatch(closeDialog())}>Done</button>
        </div>
      </div>
    </div>
  )
}
