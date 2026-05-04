import fs from 'node:fs'
import path from 'node:path'
import { XMLParser } from 'fast-xml-parser'

export interface FileResult {
  filePath: string
  data: string
  isLegacy: boolean
}

function parseLegacyTurnwatcher(xmlContent: string): object {
  const parser = new XMLParser({
    ignoreAttributes: false,
    attributeNamePrefix: '@_',
    allowBooleanAttributes: true,
  })
  const parsed = parser.parse(xmlContent)

  const characters: object[] = []
  const root = parsed.propbag

  // Extract version
  let version = '1.4'
  if (root.string) {
    const strings = Array.isArray(root.string) ? root.string : [root.string]
    const versionStr = strings.find((s: any) => s['@_name'] === 'VERSION')
    if (versionStr) {
      version = versionStr['#text'] || '1.4'
    }
  }

  // Extract characters array
  const charArray = root.array
  if (charArray) {
    const items = charArray.propbag_item
    const charItems = Array.isArray(items) ? items : items ? [items] : []

    for (const charItem of charItems) {
      const character: any = {
        id: crypto.randomUUID(),
        name: '',
        publicName: '',
        notes: '',
        monster: false,
        baseHP: 0,
        tempHP: 0,
        damage: 0,
        hitDice: 1,
        status: 0,
        stabilized: false,
        justDropped: false,
        position: 0,
        subPosition: 0,
        manualPos: 0,
        stats: {},
        effects: [],
      }

      // Parse int properties
      const ints = Array.isArray(charItem.int) ? charItem.int : charItem.int ? [charItem.int] : []
      for (const intProp of ints) {
        const name = intProp['@_name']
        const value = parseInt(intProp['@_value'], 10)
        switch (name) {
          case 'DAMAGE': character.damage = value; break
          case 'MONSTER': character.monster = value !== 0; break
          case 'STATUS': character.status = value; break
          case 'STABILIZED': character.stabilized = value !== 0; break
          case 'JUSTDROPPED': character.justDropped = value !== 0; break
          case 'POSITION': character.position = value; break
          case 'TEMPHP': character.tempHP = value; break
          case 'HITDICE': character.hitDice = value; break
          case 'BASEHP': character.baseHP = value; break
          case 'SUBPOSITION': character.subPosition = value; break
          case 'MANUALPOS': character.manualPos = value; break
        }
      }

      // Parse string properties
      const strings = Array.isArray(charItem.string) ? charItem.string : charItem.string ? [charItem.string] : []
      for (const strProp of strings) {
        const name = strProp['@_name']
        const value = strProp['#text'] || ''
        switch (name) {
          case 'NAME': character.name = value; break
          case 'PUBLIC_NAME': character.publicName = value; break
          case 'NOTES': character.notes = value; break
        }
      }

      // Parse stats array
      const statsArray = charItem.array
      if (statsArray) {
        const statItems = statsArray.propbag_item
        const statList = Array.isArray(statItems) ? statItems : statItems ? [statItems] : []
        for (const statItem of statList) {
          let statId = ''
          let modifier = 0
          let roll = 0
          let statNotes = ''

          const statInts = Array.isArray(statItem.int) ? statItem.int : statItem.int ? [statItem.int] : []
          for (const si of statInts) {
            if (si['@_name'] === 'MODIFIER') modifier = parseInt(si['@_value'], 10)
            if (si['@_name'] === 'ROLL') roll = parseInt(si['@_value'], 10)
          }

          const statStrings = Array.isArray(statItem.string) ? statItem.string : statItem.string ? [statItem.string] : []
          for (const ss of statStrings) {
            if (ss['@_name'] === 'STATID') statId = ss['#text'] || ''
            if (ss['@_name'] === 'NOTES') statNotes = ss['#text'] || ''
          }

          if (statId) {
            character.stats[statId] = { mod: modifier, roll, notes: statNotes }
          }
        }
      }

      // Parse effects
      const effectsBag = charItem.propbag
      if (effectsBag) {
        const effectBags = Array.isArray(effectsBag.propbag)
          ? effectsBag.propbag
          : effectsBag.propbag
            ? [effectsBag.propbag]
            : []

        for (const eBag of effectBags) {
          const effect: any = {
            id: crypto.randomUUID(),
            name: '',
            description: '',
            type: 0,
            totalRounds: 0,
            roundsUsed: 0,
            startIn: 0,
            hpBoost: 0,
            isActive: true,
          }

          const eInts = Array.isArray(eBag.int) ? eBag.int : eBag.int ? [eBag.int] : []
          for (const ei of eInts) {
            const name = ei['@_name']
            const value = parseInt(ei['@_value'], 10)
            switch (name) {
              case 'TYPE': effect.type = value; break
              case 'TOTALROUNDS': effect.totalRounds = value; break
              case 'ROUNDSUSED': effect.roundsUsed = value; break
              case 'STARTIN': effect.startIn = value; break
              case 'HPBOOST': effect.hpBoost = value; break
              case 'ACTIVE': effect.isActive = value !== 0; break
            }
          }

          const eStrings = Array.isArray(eBag.string) ? eBag.string : eBag.string ? [eBag.string] : []
          for (const es of eStrings) {
            if (es['@_name'] === 'NAME') effect.name = es['#text'] || ''
            if (es['@_name'] === 'DESCRIPTION') effect.description = es['#text'] || ''
          }

          character.effects.push(effect)
        }
      }

      characters.push(character)
    }
  }

  return {
    version: '2.0',
    legacyVersion: version,
    settings: {},
    characters,
    initiative: {
      inRounds: false,
      roundNumber: 0,
      currentInit: 0,
    },
  }
}

export const fileManager = {
  loadFile(filePath: string): FileResult {
    const content = fs.readFileSync(filePath, 'utf-8')
    const ext = path.extname(filePath).toLowerCase()

    if (ext === '.turnwatcher' || ext === '.xml') {
      // Legacy format — parse XML and convert to JSON
      const converted = parseLegacyTurnwatcher(content)
      return {
        filePath,
        data: JSON.stringify(converted, null, 2),
        isLegacy: true,
      }
    }

    // Native JSON format
    return {
      filePath,
      data: content,
      isLegacy: false,
    }
  },

  saveFile(filePath: string, data: string): { filePath: string; success: boolean } {
    fs.writeFileSync(filePath, data, 'utf-8')
    return { filePath, success: true }
  },
}
