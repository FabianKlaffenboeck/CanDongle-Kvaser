import {AdapterInfo, CanDevicesInterface, CanInterface, CanMessage, OpenOptions} from "@fklab/candongle-interface";

const kvaserCan = require('../build/Release/kvaser_can.node');

export type WindowsCanKvaserInterface = CanDevicesInterface<WindowsCanDeviceKvaser, OpenOptions>

export const WindowsCanKvaser: WindowsCanKvaserInterface = {

    // List available CAN devices (for simplicity, mock device info)
    async list(): Promise<AdapterInfo[]> {
        const devices = await kvaserCan.listChannels();

        return devices.map((device: any) => ({
            path: device
        }))
    },

    // Open a specific CAN device by its path and baud rate
    async open(options: OpenOptions): Promise<WindowsCanDeviceKvaser> {
        const canChannel = new WindowsCanDeviceKvaser(options.path, options.baudRate);
        await canChannel.open();
        return canChannel;
    }
}


/**
 * The Windows binding layer
 */
export class WindowsCanDeviceKvaser implements CanInterface {
    private channel: any;
    public isOpen: boolean = false;
    private messageCallback: (id: number, data: number[], length: number) => void; // Store the callback

    constructor(private path: string, private baudRate: number) {
    }

    // Open the CAN channel
    async open(): Promise<void> {
        if (this.isOpen) {
            throw new Error('CAN channel is already open');
        }

        this.channel = new kvaserCan.CanChannel(this.path, this.baudRate);
        await this.channel.openChannel();
        this.isOpen = true;
    }

    // Close the CAN channel
    async close(): Promise<void> {
        if (!this.isOpen) {
            throw new Error('CAN channel is not open');
        }

        await this.channel.closeChannel();
        this.isOpen = false;
    }

    // Read the next CAN message from the channel
    async read(): Promise<CanMessage> {
        return new Promise((resolve, reject) => {
            this.channel.setMessageCallback((msg: CanMessage) => {
                const canMessage: CanMessage = {
                    id: msg.id,
                    data: msg.data,
                    time: msg.time,
                };

                resolve(canMessage);
            });
        });
    }

    // Write a CAN message to the channel
    async write(buffer: number[]): Promise<void> {
        if (!this.isOpen) {
            throw new Error('CAN channel is not open');
        }

        await this.channel.sendMessage(buffer);
    }

    // Register a callback to be triggered when a CAN message arrives
    setMessageCallback(callback: (id: number, data: number[], length: number) => void): void {
        this.messageCallback = callback; // Store the callback

        // Register the callback with the native module
        kvaserCan.setMessageCallback((id: number, data: number[], length: number) => {
            if (this.messageCallback) {
                this.messageCallback(id, data, length); // Call the stored callback
            }
        });
    }
}

