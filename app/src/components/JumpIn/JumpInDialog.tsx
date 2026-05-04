import { useState } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { setCharacterStatus } from '../../store/slices/charactersSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { Status, Character } from '../../types'

/**
 * Jump In dialog — mirrors C++ RoundsHelper::JumpIn() / JumpInWindow.
 *
 * Lists all characters currently in Delayed or Readied status.
 * The selected character is un-delayed (status → Normal) and moved
 * to the current initiative position so they act right now.
 *
 * If there is only one such character the list is shown pre-selected.
 * If there are none the dialog shows an informational message.
 */
export function JumpInDialog() {
  const dispatch = useAppDispatch()
  const characters = useAppSelector(state => state.characters.present.list)

  // Collect characters waiting to jump in
  const waiting: Character[] = characters.filter(
    (c: Character) => !c.deleted && (c.status === Status.Delayed || c.status === Status.Readied),
  )

  // Pre-select the only character when exactly one is waiting
  const [selectedId, setSelectedId] = useState<string | null>(
    waiting.length === 1 ? waiting[0].id : null,
  )

  const handleOk = () => {
    if (!selectedId) return
    // Reset the character's status — they return to their original initiative slot
    dispatch(setCharacterStatus({ id: selectedId, status: Status.Normal }))
    dispatch(closeDialog())
  }

  const handleCancel = () => {
    dispatch(closeDialog())
  }

  return (
    <div className="modal-overlay" onClick={handleCancel}>
      <div className="modal" style={{ width: 400 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>Jump In</span>
          <button onClick={handleCancel}>×</button>
        </div>

        <div className="modal-body">
          {waiting.length === 0 ? (
            <p style={{ color: 'var(--tw-text-muted)' }}>
              No delayed or readied characters.
            </p>
          ) : (
            <>
              <p style={{ marginBottom: 8 }}>Select a character to jump in:</p>
              <div style={{ border: '1px solid var(--tw-border)', borderRadius: 4, overflow: 'hidden' }}>
                <table className="character-table" style={{ margin: 0 }}>
                  <thead>
                    <tr>
                      <th>Character Waiting</th>
                      <th style={{ width: 80 }}>Status</th>
                    </tr>
                  </thead>
                  <tbody>
                    {waiting.map(ch => (
                      <tr
                        key={ch.id}
                        className={selectedId === ch.id ? 'selected' : ''}
                        onClick={() => setSelectedId(ch.id)}
                        style={{ cursor: 'pointer' }}
                      >
                        <td>{ch.name}</td>
                        <td>{ch.status === Status.Readied ? 'Readied' : 'Delayed'}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </>
          )}
        </div>

        <div className="modal-footer">
          <button className="btn" onClick={handleCancel}>Cancel</button>
          {waiting.length > 0 && (
            <button
              className="btn btn-primary"
              disabled={!selectedId}
              onClick={handleOk}
            >
              OK
            </button>
          )}
        </div>
      </div>
    </div>
  )
}
