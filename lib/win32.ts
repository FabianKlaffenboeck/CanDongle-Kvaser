import {AdapterInfo, CanDevicesInterface, CanInterface, CanMessage, OpenOptions} from "@fklab/candongle-interface";
import {
    asyncClose,
    asyncListCanDevices,
    asyncOpenCanChannel,
    asyncRead,
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

        try {
            const handle = await asyncOpenCanChannel(options.path, options);
            return new WindowsCanDeviceKvaser(handle, options);
        } catch (error) {
            console.error('Error while opening CAN channel:', error);
            throw error;
        }
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
    }

    // TODO Implement
    async read(): Promise<CanMessage> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }

        try {
            const message = await asyncRead(this.handle);
            return {
                id: message.id,
                dlc: message.dlc,
                data: Array.from(message.data), // Convert to regular array for JavaScript compatibility
            };
        } catch (error) {
            throw new Error(`Failed to read from CAN channel: ${error.message}`);
        }
    }

    // TODO Implement
    setMessageCallback(callback: (id: number, data: number[], length: number) => void): void {
    }

    async write(buffer: CanMessage[]): Promise<void> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }

        try {
            await asyncWrite(this.handle, buffer);
        } catch (error) {
            throw new Error(`Failed to write to CAN channel: ${error.message}`);
        }
    }
}

