import device_page_styles from "./device.module.css";

import { useNavigate, useParams } from "react-router-dom"
import DevicePreview from "../../components/device_preview/device_preview";
import ButtonWidget from "../../components/button_widget/button_widget";

export default function Device() {

    let { device_id } = useParams()
    const navigate = useNavigate()

    const return_home_handler = () => {
        navigate("/")
    }

    return (
        <div className={device_page_styles.device_page}>
            <div className={device_page_styles.device_configuration}>
                <DevicePreview id={4} settings_widget_disable />
                <div>
                    <div>
                        <p>

                        </p>
                    </div>
                </div>
            </div>
            <div>
                <div className={device_page_styles.page_header}>

                </div>
            </div>
        </div>
    )

}