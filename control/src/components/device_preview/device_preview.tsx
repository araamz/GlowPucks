import { useEffect, useState } from "react";
import { Navigate, useNavigate } from "react-router-dom";
import BatteryWidget from "../battery_widget/battery_widget";
import Graphic from "../graphic/graphic";
import LightActiveWidget from "../light_active_widget/light_active_widget";
import SettingsWidget from "../settings_widget/settings_widget";
import device_preview_styles from "./device_preview.module.css"; 
import io from 'socket.io-client'

type device_preview_props = {
    uuid: any;
    light_active_widget_disable?: boolean;
    settings_widget_disable?: boolean;
}

// Will grab from API (socketio) for light_active, and label - Only UUID will be required
// Will grab from API (socketio) for r1, g1, b1 
const socket_url = "http://127.0.0.1:5000"
const socket = io(socket_url, {
    withCredentials: false
})


export default function DevicePreview(props: device_preview_props) {

    const navigate = useNavigate()

    const [r1, set_r1] = useState<Number>(0)
    const [g1, set_g1] = useState<Number>(0)
    const [b1, set_b1] = useState<Number>(0)
    const [active, set_active] = useState<Number>(0)
    const [label, set_label] = useState<String>("Glowpuck")
    const [primary_color, set_primary_color] = useState<string>("rgb(255,255,255)")
    const [device_status_object, set_device_status_object] = useState<any>()

    const settings_handler = () => {
  
        navigate(`/devices/${props.uuid}`)
        
    }

    useEffect(() => {

        set_primary_color(`rgb(${r1}, ${g1}, ${b1})`)

    }, [r1, g1, b1])

    const [socket_connected, set_socket_connected] = useState<any>(socket.connected)

    const request_status = () => {
        console.log("Requesting")
        socket.emit("status_request", props.uuid)
    }

    useEffect(() => {

        socket.on('status_request_response', (status_message) => {
            if (status_message.uuid == props.uuid) {
                set_label(status_message.label)
                set_active(status_message.active)
                set_r1(status_message.r1)
                set_g1(status_message.g1)
                set_b1(status_message.b1)
            }
        })

        return () => {
            socket.off('status_request_response')
        }

    }, [label, set_active, set_r1, set_g1, set_b1])

    useEffect(() => {
        const interval = setInterval(() => {
            request_status()
        }, 3000)


        return () => clearInterval(interval)
    }, [])



    // Connect to Status API using UUID to get data
    // Use Status API data to set device_status_object

    return (
        <div className={device_preview_styles.device_preview}>
            <div className={device_preview_styles.device_preview_container}>
                <div className={device_preview_styles.label_container}>
                    <p className={device_preview_styles.label}>
                        { label }
                    </p>
                </div>
                <div className={device_preview_styles.graphic_container}>
                    <Graphic color={primary_color} active={active} />
                </div>
                <div className={device_preview_styles.widgets_container}>
                    { !props.light_active_widget_disable ? <LightActiveWidget active={active}/> : null }
                    { !props.settings_widget_disable ? <SettingsWidget onClick={settings_handler} className={device_preview_styles.settings_widget} /> : null }
                </div>
            </div>
        </div>
    )

}