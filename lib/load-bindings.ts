import nodeGypBuild from 'node-gyp-build'
import {promisify} from 'util'
import {join} from 'path'

// Load the native binding using node-gyp-build
const binding = nodeGypBuild(join(__dirname, '../')) as any;

// Promisify the native methods or provide a fallback that throws an error
export const asyncListCanDevices = binding.list  ? promisify(binding.list ) : async () => { throw new Error('"binding.list " Method not implemented')}
export const asyncOpenCanChannel = binding.open  ? promisify(binding.open ) : async () => { throw new Error('"binding.open " Method not implemented')}
export const asyncClose = binding.close  ? promisify(binding.close ) : async () => { throw new Error('"binding.close " Method not implemented')}
export const asyncRead = binding.read  ? promisify(binding.read ) : async () => { throw new Error('"binding.read " Method not implemented')}
export const asyncSetMessageCallback = binding.setCallback  ? promisify(binding.setCallback ) : async () => { throw new Error('"binding.setCallback " Method not implemented')}
export const asyncWrite = binding.write  ? promisify(binding.write ) : async () => { throw new Error('"binding.write " Method not implemented')}
