import DevicePreview from "../../components/device_preview/device_preview";
import GroupPreview from "../../components/group_preview/group_preview";

import home_page_styles from "./home.module.css";

export default function Home() {

    return (
        <div className={home_page_styles.home}>
            <div className={home_page_styles.connected_devices}>
                <h2>
                    Connected Devices
                </h2>
                <div className={home_page_styles.devices_container}>
                    <DevicePreview battery_widget_disable uuid={0}/>


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