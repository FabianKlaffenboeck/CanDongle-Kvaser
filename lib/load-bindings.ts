import nodeGypBuild from 'node-gyp-build'
import {promisify} from 'util'
import {join} from 'path'

// Load the native binding using node-gyp-build
import path from "path";

const binding = nodeGypBuild(path.join(__dirname, 'node_modules/@fklab/candongle-kvaser')) as any;

// Promisify the native methods or provide a fallback that throws an error
export const asyncClose = binding.close ? promisify(binding.close) : async () => {
    throw new Error('"binding.close" Method not implemented');
};
export const asyncWrite = binding.write ? promisify(binding.write) : async () => {
    throw new Error('"binding.write" Method not implemented');
};
export const asyncRead = binding.read ? promisify(binding.read) : async () => {
    throw new Error('"binding.read" Method not implemented');
};
export const asyncListCanDevices = binding.listCanDevices ? promisify(binding.listCanDevices) : async () => {
    throw new Error('"binding.listCanDevices" Method not implemented');
};
export const asyncOpenCanChannel = binding.openCanChannel ? promisify(binding.openCanChannel) : async () => {
    throw new Error('"binding.openCanChannel" Method not implemented');
};
export const asyncSetMessageCallback = binding.setMessageCallback ? promisify(binding.setMessageCallback) : async () => {
    throw new Error('"binding.setMessageCallback" Method not implemented');
};
