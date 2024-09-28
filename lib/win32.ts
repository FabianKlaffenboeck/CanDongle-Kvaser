import {AdapterInfo, CanDevicesInterface, CanInterface, CanMessage, OpenOptions} from "@fklab/candongle-interface";
import {
    asyncClose,
    asyncListCanDevices,
    asyncOpenCanChannel,
    asyncRead,
    asyncSetMessageCallback, asyncWrite
} from "./load-bindings";


export type WindowsCanKvaserInterface = CanDevicesInterface<WindowsCanDeviceKvaser, OpenOptions>

export const WindowsCanKvaser: WindowsCanKvaserInterface = {

    async list(): Promise<AdapterInfo[]> {
        const devices = await asyncListCanDevices();
        return devices.map((device: any) => ({
            name: device.name,
        }));
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
    private handle: number;
    public isOpen: boolean;

    constructor(handle: number) {
        this.handle = handle;
        this.isOpen = true;
    }

    async close(): Promise<void> {
        if (!this.isOpen) {
            throw new Error("CAN channel is already closed");
        }
        await asyncClose(this.handle);
        this.isOpen = false;
    }

    async read(): Promise<CanMessage> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }
        const message = await asyncRead(this.handle);
        return {
            id: message.id,
            data: Array.from(message.data),
            time: message.time,
        };
    }

    setMessageCallback(callback: (id: number, data: number[], length: number) => void): void {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }
        asyncSetMessageCallback(this.handle, callback);
    }

    async write(buffer: number[]): Promise<void> {
        if (!this.isOpen) {
            throw new Error("CAN channel is closed");
        }
        await asyncWrite(this.handle, buffer);
    }
}

