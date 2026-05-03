import { useAppDispatch } from '../../store'
import { closeDialog } from '../../store/slices/uiSlice'

export function AboutDialog() {
  const dispatch = useAppDispatch()

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 420 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>About Turn Watcher</span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>
        <div className="modal-body">
          <div className="about-content">
            <img src="/assets/turnwatcher-splash.png" alt="Turn Watcher" />
            <h2>Turn Watcher™</h2>
            <p>Version 2.0.0</p>
            <p style={{ marginTop: 12 }}>
              A tabletop RPG combat initiative tracker.
            </p>
            <p style={{ marginTop: 8, fontSize: 11 }}>
              © 2005–2026 Made to Order Software Corporation
            </p>
            <p style={{ fontSize: 11 }}>
              Licensed under the GNU General Public License v2.0
            </p>
            <p style={{ marginTop: 12, fontSize: 11 }}>
              <a href="https://www.turnwatcher.com" target="_blank" rel="noopener noreferrer" style={{ color: 'var(--tw-accent)' }}>
                www.turnwatcher.com
              </a>
            </p>
          </div>
        </div>
        <div className="modal-footer">
          <button className="btn btn-primary" onClick={() => dispatch(closeDialog())}>OK</button>
        </div>
      </div>
    </div>
  )
}
