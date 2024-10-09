import {AdapterInfo, CanDevicesInterface, CanInterface, CanMessage, OpenOptions} from "@fklab/candongle-interface";
import {
    asyncClose,
    asyncListCanDevices,
    asyncOpenCanChannel,
    asyncSetMessageCallback,
    asyncWrite
} from "./load-bindings";


export type WindowsCanKvaserInterface = CanDevicesInterface<WindowsCanDeviceKvaser, OpenOptions>

export const WindowsCanKvaser: WindowsCanKvaserInterface = {

    async list(): Promise<AdapterInfo[]> {
        return asyncListCanDevices();
    },
    async open(options: OpenOptions): Promise<WindowsCanDeviceKvaser> {

        if (!options || typeof options !== 'object' || Array.isArray(options)) {
            throw new TypeError('"options" is not an object')
        }
        if (options.path == undefined) {
            throw new TypeError('"path" is not a valid port')
        }
        if (options.baudRate == undefined) {
            throw new TypeError('"baudRate" is not a valid baudRate')
        }

        const handle = await asyncOpenCanChannel(options.path, options);
        return new WindowsCanDeviceKvaser(handle, options);
    }
}


/**
 * The Windows binding layer
 */
export class WindowsCanDeviceKvaser implements CanInterface {
    private readonly handle: null | number;  // Handle for the CAN channel
    public openOptions: Required<OpenOptions>

    constructor(handle: number, options: Required<OpenOptions>) {
        this.handle = handle;
        this.openOptions = options
    }

    get isOpen() {
        return this.handle !== null
    }

    async close(): Promise<void> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }
        await asyncClose(this.handle);
    }

    // @ts-ignore
    async read(): Promise<CanMessage> {
    }

    setMessageCallback(callback: (id: number, data: number[], length: number) => void): void {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }
        asyncSetMessageCallback(this.handle, callback)
    }

    async write(buffer: CanMessage[]): Promise<void> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }

        await asyncWrite(this.handle, buffer);
    }
}

