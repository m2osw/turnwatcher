// ============================================================================
// Turn Watcher — Core Type Definitions
// Mirrors the C++ data model from Combatant::Character, Effects::Effect,
// Attribute::Stat, Application::AppSettings, Initiative::InitiativeManager
// ============================================================================

// Character health status (from C++ Combatant::Health enum)
export enum Health {
  Normal = 'Normal',
  Stabilized = 'Stabilized',
  Disabled = 'Disabled',
  Dying = 'Dying',
  Dead = 'Dead',
}

// Character action status (from C++ Combatant::Status enum)
export enum Status {
  Normal = 0,
  Delayed = 1,
  Readied = 2,
}

// Effect type (from C++ Effects::Effect::Type enum)
export enum EffectType {
  Spell = 0,
  Ability = 1,
  Other = 2,
}

// An individual stat value on a character (from C++ Attribute::Value)
export interface StatValue {
  mod: number
  roll: number
  notes: string
}

// A stat definition (from C++ Attribute::Stat)
export interface StatDefinition {
  id: string
  name: string
  abilityId: string
  dice: number
  faces: number
  modifier: number
  accel: string
  showOnToolbar: boolean
  showOnHUD: boolean
  showMonsterOnHUD: boolean
  internal: boolean
  ability: boolean
  order: number
  deleted: boolean
}

// An active effect on a character (from C++ Effects::Effect)
export interface Effect {
  id: string
  name: string
  description: string
  type: EffectType
  totalRounds: number
  roundsUsed: number
  startIn: number
  tempHP: number
  hpBoost: number
  isActive: boolean
}

// A character / combatant (from C++ Combatant::Character)
export interface Character {
  id: string
  name: string
  publicName: string
  notes: string
  monster: boolean
  baseHP: number
  tempHP: number
  damage: number
  hitDice: number
  status: Status
  stabilized: boolean
  justDropped: boolean
  position: number
  subPosition: number
  manualPos: number
  deleted: boolean
  stats: Record<string, StatValue>
  effects: Effect[]
}

// Initiative state (from C++ Initiative::InitiativeManager)
export interface InitiativeState {
  inRounds: boolean
  roundNumber: number
  currentInit: number
}

// Application settings (from C++ Application::AppSettings)
export interface AppSettings {
  ultraInit: boolean
  bleedOutDying: boolean
  skipDead: boolean
  altDeath: boolean
  notifyExpiredEffects: boolean
  showToolbar: boolean
  rollInitOnStart: boolean
  manualInitiative: boolean
  getDC: boolean
  lastDC: number
  showEffects: boolean
  showInfo: boolean
  showHUD: boolean
  deathThreshold: number
  combatantListFont: string
  altCombatantListFont: string
  initDieStrings: string[]
  toolbarPos: 'top' | 'bottom'
}

// The complete save file format
export interface SaveFile {
  version: string
  legacyVersion?: string
  settings: Partial<AppSettings>
  characters: Character[]
  stats?: StatDefinition[]
  initiative: InitiativeState
}

// Electron API exposed via preload
export interface ElectronAPI {
  fileOpen: () => Promise<{ filePath: string; data: string; isLegacy: boolean } | null>
  fileSave: (data: string, filePath?: string) => Promise<{ filePath: string; success: boolean } | null>
  hudOpen: () => Promise<void>
  hudClose: () => Promise<void>
}

declare global {
  interface Window {
    electronAPI?: ElectronAPI
  }
}
