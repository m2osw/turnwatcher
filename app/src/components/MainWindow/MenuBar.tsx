import { useState, useRef, useEffect } from 'react'
import { useAppDispatch, useAppSelector } from '../../store'
import { ActionCreators } from 'redux-undo'
import { clearCharacters, purgeDead } from '../../store/slices/charactersSlice'
import { openDialog } from '../../store/slices/uiSlice'

interface MenuBarProps {
  onFileOpen: () => void
  onFileSave: (saveAs?: boolean) => void
}

export function MenuBar({ onFileOpen, onFileSave }: MenuBarProps) {
  const dispatch = useAppDispatch()
  const [activeMenu, setActiveMenu] = useState<string | null>(null)
  const menuRef = useRef<HTMLDivElement>(null)
  const canUndo = useAppSelector(state => state.characters.past.length > 0)
  const canRedo = useAppSelector(state => state.characters.future.length > 0)
  const initiative = useAppSelector(state => state.initiative)
  const showHUD = useAppSelector(state => state.settings.showHUD)

  useEffect(() => {
    function handleClickOutside(e: MouseEvent) {
      if (menuRef.current && !menuRef.current.contains(e.target as Node)) {
        setActiveMenu(null)
      }
    }
    document.addEventListener('mousedown', handleClickOutside)
    return () => document.removeEventListener('mousedown', handleClickOutside)
  }, [])

  const toggleMenu = (name: string) => {
    setActiveMenu(prev => (prev === name ? null : name))
  }

  const menuAction = (action: () => void) => {
    setActiveMenu(null)
    action()
  }

  return (
    <div className="menu-bar" ref={menuRef}>
      {/* File Menu */}
      <div className="menu-bar-item">
        <button onClick={() => toggleMenu('file')}>File</button>
        {activeMenu === 'file' && (
          <div className="menu-dropdown">
            <button onClick={() => menuAction(onFileOpen)}>
              Open... <span className="menu-shortcut">Ctrl+O</span>
            </button>
            <button onClick={() => menuAction(() => onFileSave(false))}>
              Save <span className="menu-shortcut">Ctrl+S</span>
            </button>
            <button onClick={() => menuAction(() => onFileSave(true))}>
              Save As... <span className="menu-shortcut">Ctrl+Shift+S</span>
            </button>
            <div className="menu-separator" />
            <button onClick={() => menuAction(() => dispatch(clearCharacters()))}>
              Clear All
            </button>
            <div className="menu-separator" />
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'preferences' })))}>
              Preferences...
            </button>
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'statManager' })))}>
              Stat Manager...
            </button>
          </div>
        )}
      </div>

      {/* Edit Menu */}
      <div className="menu-bar-item">
        <button onClick={() => toggleMenu('edit')}>Edit</button>
        {activeMenu === 'edit' && (
          <div className="menu-dropdown">
            <button disabled={!canUndo} onClick={() => menuAction(() => dispatch(ActionCreators.undo()))}>
              Undo <span className="menu-shortcut">Ctrl+Z</span>
            </button>
            <button disabled={!canRedo} onClick={() => menuAction(() => dispatch(ActionCreators.redo()))}>
              Redo <span className="menu-shortcut">Ctrl+Y</span>
            </button>
            <div className="menu-separator" />
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'editCharacter', characterId: undefined })))}>
              Add Character... <span className="menu-shortcut">Ctrl+A</span>
            </button>
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'editCharacter' })))}>
              Edit Character...
            </button>
            <button onClick={() => menuAction(() => dispatch(purgeDead()))}>
              Purge Dead
            </button>
          </div>
        )}
      </div>

      {/* View Menu */}
      <div className="menu-bar-item">
        <button onClick={() => toggleMenu('view')}>View</button>
        {activeMenu === 'view' && (
          <div className="menu-dropdown">
            <button onClick={() => menuAction(() => {
              if (window.electronAPI) {
                showHUD ? window.electronAPI.hudClose() : window.electronAPI.hudOpen()
              }
            })}>
              {showHUD ? '✓ ' : ''}Show Player HUD
            </button>
          </div>
        )}
      </div>

      {/* Rounds Menu */}
      <div className="menu-bar-item">
        <button onClick={() => toggleMenu('rounds')}>Rounds</button>
        {activeMenu === 'rounds' && (
          <div className="menu-dropdown">
            <button disabled={initiative.inRounds} onClick={() => menuAction(() => {
              const { startRounds } = require('../../store/slices/initiativeSlice')
              dispatch(startRounds())
            })}>
              Start
            </button>
            <button disabled={!initiative.inRounds} onClick={() => menuAction(() => {
              const { endRounds } = require('../../store/slices/initiativeSlice')
              dispatch(endRounds())
            })}>
              End
            </button>
            <button disabled={!initiative.inRounds} onClick={() => menuAction(() => {
              // Next turn handled by toolbar
            })}>
              Next <span className="menu-shortcut">Ctrl+N</span>
            </button>
            <div className="menu-separator" />
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'damageDialog' })))}>
              Damage... <span className="menu-shortcut">Ctrl+D</span>
            </button>
          </div>
        )}
      </div>

      {/* Roll Menu */}
      <div className="menu-bar-item">
        <button onClick={() => toggleMenu('roll')}>Roll</button>
        {activeMenu === 'roll' && (
          <div className="menu-dropdown">
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'initRollDialog' })))}>
              Initiative...
            </button>
          </div>
        )}
      </div>

      {/* Help Menu */}
      <div className="menu-bar-item">
        <button onClick={() => toggleMenu('help')}>Help</button>
        {activeMenu === 'help' && (
          <div className="menu-dropdown">
            <button onClick={() => menuAction(() => dispatch(openDialog({ dialog: 'aboutDialog' })))}>
              About Turn Watcher
            </button>
          </div>
        )}
      </div>
    </div>
  )
}
