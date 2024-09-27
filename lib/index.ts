/* eslint-disable @typescript-eslint/no-var-requires */
import {WindowsCanKvaser, WindowsCanKvaserInterface} from "./win32";


// export * from './darwin'
// export * from './linux'
export * from './win32'

export type AutoDetectTypes = WindowsCanKvaserInterface

/**
 * This is an auto detected binding for your current platform
 */
export function autoDetect(): AutoDetectTypes {
    switch (process.platform) {
        case 'win32':
            debug('loading WindowsBinding')
            return WindowsCanKvaser
        // case 'darwin':
        //   debug('loading DarwinBinding')
        //   return DarwinBinding
        // default:
        //   debug('loading LinuxBinding')
        //   return LinuxBinding
        default:
            debug('loading WindowsBinding')
            return WindowsCanKvaser
    }
}

