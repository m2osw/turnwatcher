import { useState } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { setCharacterStatus, updateCharacter } from '../../store/slices/charactersSlice'
import { closeDialog } from '../../store/slices/uiSlice'
import { Status, Character } from '../../types'
import { sortByInitiative } from '../../utils/initiative'

/**
 * Jump In dialog — mirrors C++ RoundsHelper::JumpIn() / JumpInWindow.
 *
 * Lists all characters currently in Delayed or Readied status.
 * On OK the selected character is:
 *   1. Un-delayed (status → Normal)
 *   2. Moved to act immediately after the current-initiative character
 *      by splicing them into the sorted order and reassigning position
 *      values (1..N) to the entire list — exactly mirroring the C++
 *      InternalMoveCharacter + SetPositions logic.
 *
 * If there is only one such character the list is shown pre-selected.
 * If there are none the dialog shows an informational message.
 */
export function JumpInDialog() {
  const dispatch = useAppDispatch()
  const characters = useAppSelector(state => state.characters.present.list)
  const initiative = useAppSelector(state => state.initiative)

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

    const activeChars = characters.filter((c: Character) => !c.deleted)
    const sorted = sortByInitiative(activeChars)

    // Find the index of the current-initiative character
    const currentIdx = sorted.findIndex((c: Character) => {
      const pos = c.manualPos > 0 ? c.manualPos : c.position
      return pos === initiative.currentInit
    })

    // Find the jumped-in character
    const jumpChar = sorted.find((c: Character) => c.id === selectedId)
    if (!jumpChar) {
      dispatch(closeDialog())
      return
    }

    // Build new order: remove jumpChar from wherever they are, insert right
    // after the current-initiative character (mirroring C++ MoveCharacter)
    const withoutJump = sorted.filter((c: Character) => c.id !== selectedId)
    const insertAfter = currentIdx === -1 ? withoutJump.length : currentIdx
    const reordered = [
      ...withoutJump.slice(0, insertAfter + 1),
      jumpChar,
      ...withoutJump.slice(insertAfter + 1),
    ]

    // Reassign positions sequentially (highest position = first in order)
    // matching the C++ SetPositions() which assigns 0-based index → we use
    // 1-based descending so that position N acts before position N-1.
    const total = reordered.length
    reordered.forEach((c: Character, idx: number) => {
      const newPosition = total - idx
      if (c.position !== newPosition || c.manualPos !== 0) {
        dispatch(updateCharacter({ id: c.id, changes: { position: newPosition, manualPos: 0 } }))
      }
    })

    // Reset the jumped-in character's status
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
