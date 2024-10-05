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
        const handle = await asyncOpenCanChannel(options.path, options.baudRate);
        return new WindowsCanDeviceKvaser(handle);
    }
}


/**
 * The Windows binding layer
 */
export class WindowsCanDeviceKvaser implements CanInterface {
    private handle: number;  // Handle for the CAN channel
    public isOpen: boolean;   // Flag to indicate if the channel is open

    constructor(handle: number) {
        this.handle = handle;
        this.isOpen = true; // Mark channel as open upon creation
    }

    async close(): Promise<void> {
        if (!this.isOpen) {
            throw new Error("CAN channel is already closed");
        }

        try {
            await asyncClose(this.handle);
            this.isOpen = false; // Update state after closing
        } catch (error) {
            throw new Error(`Failed to close CAN channel: ${error.message}`);
        }
    }

    async read(): Promise<CanMessage> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }

        try {
            const message = await asyncRead(this.handle);
            return {
                id: message.id,
                data: Array.from(message.data), // Convert to regular array for JavaScript compatibility
                time: message.time,
            };
        } catch (error) {
            throw new Error(`Failed to read from CAN channel: ${error.message}`);
        }
    }

    setMessageCallback(callback: (id: number, data: number[], length: number) => void): void {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }

        try {
            asyncSetMessageCallback(this.handle, callback);
        } catch (error) {
            throw new Error(`Failed to set message callback: ${error.message}`);
        }
    }

    async write(buffer: number[]): Promise<void> {
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

