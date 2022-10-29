import DevicePreview from "../../components/device_preview/device_preview";

import home_page_styles from "./home.module.css";

export default function Home() {

    return (
        <div>
            <div className={home_page_styles.connected_devices}>
                <h2>
                    Connected Devices
                </h2>
                <div className={home_page_styles.devices_container}>
                    <DevicePreview id={0} label={"Puck 1"} battery_level={20} light_active={false} />
                    <DevicePreview id={0} label={"Puck 1"} battery_level={20} light_active={false} />
                    <DevicePreview id={0} label={"Puck 1"} battery_level={20} light_active={false} />
                    <DevicePreview id={0} label={"Puck 1"} battery_level={20} light_active={false} />
                    <DevicePreview id={0} label={"Puck 1"} battery_level={20} light_active={false} />
                    <DevicePreview id={0} label={"Puck 1"} battery_level={20} light_active={false} />
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