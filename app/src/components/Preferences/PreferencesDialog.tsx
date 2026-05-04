import { useAppDispatch, useAppSelector } from '../../store'
import { updateSettings } from '../../store/slices/settingsSlice'
import { closeDialog } from '../../store/slices/uiSlice'

export function PreferencesDialog() {
  const dispatch = useAppDispatch()
  const settings = useAppSelector(state => state.settings)

  const toggle = (key: keyof typeof settings) => {
    dispatch(updateSettings({ [key]: !settings[key] }))
  }

  return (
    <div className="modal-overlay" onClick={() => dispatch(closeDialog())}>
      <div className="modal" style={{ width: 450 }} onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <span>Preferences</span>
          <button onClick={() => dispatch(closeDialog())}>×</button>
        </div>
        <div className="modal-body">
          <h3 style={{ marginBottom: 12, fontSize: 14 }}>Combat Rules</h3>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.bleedOutDying} onChange={() => toggle('bleedOutDying')} />
              <label>Bleed out when dying</label>
            </div>
          </div>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.skipDead} onChange={() => toggle('skipDead')} />
              <label>Skip dead characters in initiative</label>
            </div>
          </div>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.altDeath} onChange={() => toggle('altDeath')} />
              <label>Alternate death rule</label>
            </div>
          </div>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.notifyExpiredEffects} onChange={() => toggle('notifyExpiredEffects')} />
              <label>Notify when effects expire</label>
            </div>
          </div>

          <div className="form-group">
            <label>Death Threshold</label>
            <input
              type="number"
              value={settings.deathThreshold}
              onChange={e => dispatch(updateSettings({ deathThreshold: parseInt(e.target.value) || -10 }))}
              style={{ width: 80 }}
            />
          </div>

          <h3 style={{ marginBottom: 12, marginTop: 16, fontSize: 14 }}>Initiative</h3>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.ultraInit} onChange={() => toggle('ultraInit')} />
              <label>Ultra Initiative (re-roll each round)</label>
            </div>
          </div>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.rollInitOnStart} onChange={() => toggle('rollInitOnStart')} />
              <label>Roll initiative on start</label>
            </div>
          </div>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.manualInitiative} onChange={() => toggle('manualInitiative')} />
              <label>Manual initiative entry</label>
            </div>
          </div>

          <h3 style={{ marginBottom: 12, marginTop: 16, fontSize: 14 }}>Display</h3>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.showToolbar} onChange={() => toggle('showToolbar')} />
              <label>Show toolbar</label>
            </div>
          </div>

          <div className="form-group">
            <div className="checkbox-group">
              <input type="checkbox" checked={settings.showEffects} onChange={() => toggle('showEffects')} />
              <label>Show effects panel</label>
            </div>
          </div>
        </div>
        <div className="modal-footer">
          <button className="btn btn-primary" onClick={() => dispatch(closeDialog())}>Done</button>
        </div>
      </div>
    </div>
  )
}
