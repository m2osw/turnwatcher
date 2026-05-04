// Health computation utilities
// Mirrors C++ Character::health(), Character::maxHP(), Character::hitpoints()

import { Character, Health, Status } from '../types'

/**
 * Get the maximum HP for a character (baseHP + tempHP + effect boosts).
 */
export function getMaxHP(character: Character): number {
  let hp = character.baseHP + character.tempHP
  // Add HP boosts from active effects
  for (const effect of character.effects) {
    if (effect.isActive && effect.startIn <= 0) {
      hp += effect.hpBoost
    }
  }
  return hp
}

/**
 * Get current hit points (maxHP - damage).
 */
export function getCurrentHP(character: Character): number {
  return getMaxHP(character) - character.damage
}

/**
 * Get the health status of a character.
 * Mirrors the C++ Character::health() method.
 */
export function getHealth(character: Character, deathThreshold: number = -10): Health {
  const hp = getCurrentHP(character)
  const maxHP = getMaxHP(character)

  if (hp <= deathThreshold) {
    return Health.Dead
  }

  if (hp < 0) {
    if (character.stabilized) {
      return Health.Stabilized
    }
    return Health.Dying
  }

  if (hp === 0) {
    return Health.Disabled
  }

  return Health.Normal
}

/**
 * Get the HP percentage for display (0–100).
 */
export function getHPPercent(character: Character): number {
  const maxHP = getMaxHP(character)
  if (maxHP <= 0) return 0
  const current = getCurrentHP(character)
  if (current <= 0) return 0
  return Math.round((current / maxHP) * 100)
}

/**
 * Get a human-readable status string.
 * Matches the original UI: "100%", "Delayed", "Readied", or health percentage.
 */
export function getStatusString(character: Character, deathThreshold: number = -10): string {
  const health = getHealth(character, deathThreshold)

  if (health === Health.Dead) return 'Dead'
  if (health === Health.Dying) return 'Dying'
  if (health === Health.Stabilized) return 'Stabilized'
  if (health === Health.Disabled) return 'Disabled'

  if (character.status === Status.Delayed) return 'Delayed'
  if (character.status === Status.Readied) return 'Readied'

  return `${getHPPercent(character)}%`
}

/**
 * Get the hit points display string (e.g., "10 / 10").
 */
export function getHPDisplay(character: Character): string {
  const current = getCurrentHP(character)
  const max = getMaxHP(character)
  return `${current} / ${max}`
}
