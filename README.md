# Turn Watcher™

A tabletop RPG combat initiative tracker.

## Overview

Turn Watcher helps Game Masters manage combat encounters in tabletop RPGs like
Dungeons & Dragons. It tracks initiative order, hit points, effects, ability
scores, saves, and skills for all combatants.

## Technology Stack

- **Electron** — Desktop application shell
- **React 18** — UI framework
- **Redux Toolkit** — State management (with `redux-undo` for undo/redo)
- **Vite** — Build tool with hot module replacement
- **TypeScript** — Type-safe JavaScript

## Project Structure

```
turnwatcher/
├── app/                    # Electron + React application
│   ├── electron/           # Electron main process (main.ts, preload.ts, fileManager.ts)
│   ├── src/                # React renderer
│   │   ├── components/     # UI components
│   │   ├── store/          # Redux store and slices
│   │   ├── types/          # TypeScript type definitions
│   │   └── utils/          # Core logic (dice, initiative, health)
│   ├── public/assets/      # Icons, images, splash screen
│   └── build/              # Packaging assets (icon.icns, icon.ico, icon.png)
├── cpp/                    # Legacy C++ source (Gtkmm/Qt, archived)
├── conffiles/              # Sample .turnwatcher save files (legacy format)
├── doc/                    # Documentation and screenshots
└── images/                 # Branding images
```

## Getting Started

```bash
cd app
npm install
npm run dev
```

This starts Vite + Electron in development mode with hot reload.

## Building for Distribution

```bash
cd app
npm run electron:build
```

This produces platform-specific installers in `app/release/`.

## File Format

Turn Watcher 2.0 saves files as JSON (`.json`). It can also open legacy
`.turnwatcher` XML files from version 1.x and automatically converts them.

## License

GNU General Public License v2.0 — see [LICENSE](LICENSE).

© 2005–2026 Made to Order Software Corporation — [www.turnwatcher.com](https://www.turnwatcher.com)
