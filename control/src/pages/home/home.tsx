import DevicePreview from "../../components/device_preview/device_preview";
import GroupPreview from "../../components/group_preview/group_preview";

import home_page_styles from "./home.module.css";

export default function Home() {

    return (
        <div>
            <div className={home_page_styles.connected_devices}>
                <h2>
                    Connected Devices
                </h2>
                <div className={home_page_styles.devices_container}>
                    <DevicePreview id={0}/>
                    <DevicePreview id={1}/>
                    <DevicePreview id={2}/>
                    <DevicePreview id={2}/>
                    <DevicePreview id={2}/>
                    <DevicePreview id={2}/>
                    <DevicePreview id={2}/>


                </div>
            </div>
            <div>
                <h2>
                    Device Groups
                </h2>
                <div className={home_page_styles.groups_container}>
                    <GroupPreview id={4} />
                </div>
            </div>
        </div>
    )


}