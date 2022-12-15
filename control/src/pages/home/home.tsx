import { useEffect, useState } from "react";
import { useFetcher } from "react-router-dom";
import DevicePreview from "../../components/device_preview/device_preview";
import GroupPreview from "../../components/group_preview/group_preview";
import ButtonWidget from "../../components/button_widget/button_widget";

import home_page_styles from "./home.module.css";

export default function Home() {

    // Request UUIDs

    const [uuids, set_uuids] = useState<string[]>()
    
    const glowpucks_uuids_url = "http://127.0.0.1:5000/device/uuids"
    
    const refresh_list = () => {
        let uuids_data: any[] = []
        fetch(glowpucks_uuids_url).then(response => response.json())
        .then((data) => {
            set_uuids(data)
        })
    }

    useEffect(() => {
        console.log(uuids)
    }, [uuids])

    return (
        <div className={home_page_styles.home}>
            <div className={home_page_styles.connected_devices}>
                <div className={home_page_styles.connected_devices_header}>
                    <ButtonWidget onClick={refresh_list} className={home_page_styles.refresh_button}>
                        <span>
                            <span className={`material-symbols-outlined ${home_page_styles.refresh_icon}`}>
                                refresh
                            </span>  
                        </span>
                    </ButtonWidget>
                    <h2>
                        Connected Devices
                    </h2>
                </div>
                <div className={home_page_styles.devices_container}>

                    {uuids?.map((uuid) => (
                        <DevicePreview key={uuid} uuid={uuid} />
                    ))}

                </div>
            </div>
        </div>
    )


}