import DevicePreview from "../../components/device_preview/device_preview";

export default function Home() {

    return (
        <div>
            <div className="">
                <h2>
                    Connected Devices
                </h2>
                <div>
                    <DevicePreview />
                </div>
            </div>
            <div>
                <h2>
                    Device Groups
                </h2>
                <div>

                </div>
            </div>
        </div>
    )


}