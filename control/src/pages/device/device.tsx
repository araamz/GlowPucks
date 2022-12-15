import device_page_styles from "./device.module.css";

import { useNavigate, useParams } from "react-router-dom"
import DevicePreview from "../../components/device_preview/device_preview";
import ButtonWidget from "../../components/button_widget/button_widget";
import Graphic from "../../components/graphic/graphic";
import { useEffect, useState } from "react";
import Widget from "../../components/widget/widget";
import io, { Socket } from 'socket.io-client';

enum brightness_modes {
    dim = 20,
    normal = 100,
    bright = 255
}
enum modes {
    still = 0,
    flash = 1,
    alternate = 2
}
enum protocol_message_type {
    config = 0,
    status = 1
}

const socket_url = "http://127.0.0.1:5000"
const socket = io(socket_url, {
    withCredentials: false
})

const inital_config = {
    "label": "GP1",
    "brightness": brightness_modes.normal,
    "active": 0,
    "mode": 0,
    "r1": 255,
    "g1": 0,
    "b1": 0,
    "r2": -1,
    "g2": -1,
    "b2": -1,
    "group_enable": 0,
    "group_target": "NULL",
    "car_clear": 0,
    "car_count": 0
}

export default function Device() {

    let { device_uuid } = useParams();

    const [current_config, set_current_config] = useState<any>(inital_config)
    const [label, set_label] = useState<String>("Glowpuck")
    const [brightness, set_brightness] = useState<brightness_modes>(brightness_modes.dim)
    const [active, set_active] = useState<Number>(0)
    const [mode, set_mode] = useState<modes>(modes.still)
    const [r1, set_r1] = useState<Number>(0)
    const [g1, set_g1] = useState<Number>(0)
    const [b1, set_b1] = useState<Number>(0)
    const [r2, set_r2] = useState<Number>(-1)
    const [g2, set_g2] = useState<Number>(-1)
    const [b2, set_b2] = useState<Number>(-1)
    const [group_enable, set_group_enable] = useState<Number>(0)
    const [group_target, set_group_target] = useState<String>("NULL")
    const [car_count, set_car_count] = useState<Number>(0)

    const create_protocol_message = (type: protocol_message_type, car_clear: Number) => {

        if (car_clear == undefined) {
            car_clear = 0;
        }

        let protocol_message = {
            "uuid": device_uuid,
            "label": label,
            "brightness": brightness,
            "active": active,
            "mode": mode,
            "r1": r1,
            "g1": g1,
            "b1": b1,
            "r2": r2,
            "g2": g2,
            "b2": b2,
            "group_enable": group_enable,
            "group_target": "NULL",
            "car_clear": car_clear
        }

        return JSON.stringify(protocol_message)

    }

    const request_status = () => {
        console.log("Requesting")
        socket.emit("status_request", device_uuid)
    }

    useEffect(() => {
        const interval = setInterval(() => {
            request_status()
        }, 3000)

        return () => clearInterval(interval)
    }, [])

    useEffect(() => {

        socket.on('status_request_response', (status_message) => {
            if (status_message.uuid === device_uuid) {
                console.log(status_message)
                set_current_config(status_message)
            }
        })

        return () => {
            socket.off('status_request_response')
        }

    }, [label, set_active, set_r1, set_g1, set_b1])

    useEffect(() => {
        if (mode !== 2) {
            set_r2(-1);
            set_g2(-1);
            set_b2(-1);
        } else {
            set_r2(255)
            set_g2(0)
            set_b2(0)
        }
    }, [mode, set_r2, set_g2, set_b2])

    useEffect(() => {

        if (group_enable !== 1) {
            set_group_target("NULL")
        } 

    }, [group_enable, set_group_target])

    useEffect(() => {
        console.log(`Loading settings for device with UUID of ${device_uuid}`)
    }, [])

    const label_change = (event: any) => {
        set_label(event?.target.value)
    }

    const dim_brightness_select = () => {
        set_brightness(brightness_modes.dim)
    }
    const normal_brightness_select = () => {
        set_brightness(brightness_modes.normal)
    }
    const bright_brightness_select = () => {
        set_brightness(brightness_modes.bright)
    }

    const red_primary_color_select = () => {
        set_r1(255)
        set_g1(0)
        set_b1(0)
    }
    const orange_primary_color_select = () => {
        set_r1(255)
        set_g1(190)
        set_b1(0)
    }
    const yellow_primary_color_select = () => {
        set_r1(255)
        set_g1(255)
        set_b1(0)
    }
    const blue_primary_color_select = () => {
        set_r1(0)
        set_g1(0)
        set_b1(255)
    }
    const white_primary_color_select = () => {
        set_r1(255)
        set_g1(255)
        set_b1(255)
    }
    const red_seccondary_color_select = () => {
        set_r2(255)
        set_g2(0)
        set_b2(0)
    }
    const orange_seccondary_color_select = () => {
        set_r2(255)
        set_g2(190)
        set_b2(0)
    }
    const yellow_seccondary_color_select = () => {
        set_r2(255)
        set_g2(255)
        set_b2(0)
    }
    const blue_seccondary_color_select = () => {
        set_r2(0)
        set_g2(0)
        set_b2(255)
    }
    const white_seccondary_color_select = () => {
        set_r2(255)
        set_g2(255)
        set_b2(255)
    }

    const enable_active_select = () => {
        set_active(1)
    }
    const disable_active_select = () => {
        set_active(0)
    }

    const still_mode_select = () => {
        set_mode(modes.still)
    }
    const flash_mode_select = () => {
        set_mode(modes.flash)
    }
    const alternate_mode_select = () => {
        set_mode(modes.alternate)
    }

    const clear_car_count_click = () => {
        let message = create_protocol_message(protocol_message_type.status, 1)
        socket.emit("config_update", message)
    }
    const update_config_click = () => {
        let message = create_protocol_message(protocol_message_type.status, 0)
        console.log(message)
        socket.emit("config_update", message)
    }

    return (

        <div className={device_page_styles.device_page}>
            <div className={device_page_styles.device_information}>
                <h5>Device Information</h5>
                <div className={device_page_styles.device_attributes}>
                    <div className={device_page_styles.graphic_attribute}>
                        <Graphic active={current_config.active} color={"red"} />
                    </div>
                    <div className={device_page_styles.label_attribute}>
                        <p>
                            Label
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>
                                { inital_config.label }
                            </p>
                        </div>
                    </div>
                    <div className={device_page_styles.uuid_attribute}>
                        <p>
                            UUID
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>{ device_uuid }</p>
                        </div>
                    </div>
                    <div className={device_page_styles.brightness_attribute}>
                        <p>
                            Brightness
                        </p>
                        <div>
                            {current_config.brightness == brightness_modes.dim ? <p className={device_page_styles.attribute_value}>Dim</p> : undefined}
                            {current_config.brightness == brightness_modes.normal ? <p className={device_page_styles.attribute_value}>Normal</p> : undefined}
                            {current_config.brightness == brightness_modes.bright ? <p className={device_page_styles.attribute_value}>Bright</p> : undefined}
                        </div>
                    </div>
                    <div className={device_page_styles.mode_attribute}>
                        <p>
                            Mode
                        </p>
                        <div>
                            {current_config.mode == modes.still ? <p className={device_page_styles.attribute_value}>Still</p> : undefined}
                            {current_config.mode == modes.flash ? <p className={device_page_styles.attribute_value}>Flash</p> : undefined}
                            {current_config.mode == modes.alternate ? <p className={device_page_styles.attribute_value}>Alternate</p> : undefined}
                        </div>
                    </div>
                    <div className={device_page_styles.mode_attribute}>
                        <p>
                            Active
                        </p>
                        <div>
                            {current_config.active == 1 ? <p className={device_page_styles.attribute_value}>Enabled</p> : <p className={device_page_styles.attribute_value}>Disabled</p>}
                        </div>
                    </div>
                    <div className={device_page_styles.primary_color_attribute}>
                        <p>
                            Primary Color
                        </p>
                        <div>
                            {((Number(current_config.r1) === 255) && (Number(current_config.g1) === 0) && (Number(current_config.b1) === 0)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.red_color_option}`} /> : undefined } 
                            {((Number(current_config.r1) === 255) && (Number(current_config.g1) === 190) && (Number(current_config.b1) === 0)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.orange_color_option}`} /> : undefined } 
                            {((Number(current_config.r1) === 255) && (Number(current_config.g1) === 255) && (Number(current_config.b1) === 0)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.yellow_color_option}`} /> : undefined } 
                            {((Number(current_config.r1) === 0) && (Number(current_config.g1) === 0) && (Number(current_config.b1) === 255)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.blue_color_option}`} /> : undefined } 
                            {((Number(current_config.r1) === 255) && (Number(current_config.g1) === 255) && (Number(current_config.b1) === 255)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.white_color_option}`} /> : undefined } 
                        </div>
                    </div>
                    <div className={device_page_styles.seccondary_color_attribute}>
                        <p>
                            Seccondary Color
                        </p>
                        <div>
                            { current_config.mode == modes.alternate ? 
                                <>
                                    {((Number(current_config.r2) === 255) && (Number(current_config.g2) === 0) && (Number(current_config.b2) === 0)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.red_color_option}`} /> : undefined } 
                                    {((Number(current_config.r2) === 255) && (Number(current_config.g2) === 190) && (Number(current_config.b2) === 0)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.orange_color_option}`} /> : undefined } 
                                    {((Number(current_config.r2) === 255) && (Number(current_config.g2) === 255) && (Number(current_config.b2) === 0)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.yellow_color_option}`} /> : undefined } 
                                    {((Number(current_config.r2) === 0) && (Number(current_config.g2) === 0) && (Number(current_config.b2) === 255)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.blue_color_option}`} /> : undefined } 
                                    {((Number(current_config.r2) === 255) && (Number(current_config.g2) === 255) && (Number(current_config.b2) === 255)) ? <div className={`${device_page_styles.attribute_color} ${device_page_styles.white_color_option}`} /> : undefined } 
                                </>
                            : <div className={`${device_page_styles.attribute_color} ${device_page_styles.attribute_color_inactive}`} /> }
                        </div>
                    </div>
                    <div className={device_page_styles.car_count_attribute}>
                        <p>
                            Car Count 
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>{ String(current_config.car_count) }</p>
                        </div>
                    </div>
                </div>
            </div>
            <div className={device_page_styles.device_configuration}>
                <h5> Device Configuration </h5>
                <div className={device_page_styles.device_configuration_options}>
                    <div>
                        <p>
                            Device Label
                        </p>
                        <textarea onChange={label_change} className={device_page_styles.label_options} rows={1} maxLength={65} />
                    </div>
                    <div>
                        <p>
                            Device Brightness
                        </p>
                        <div className={device_page_styles.brightness_options}>
                            <button onClick={dim_brightness_select} className={brightness === brightness_modes.dim ? device_page_styles.option_selected : undefined}>Dim</button>
                            <button onClick={normal_brightness_select} className={brightness === brightness_modes.normal ? device_page_styles.option_selected : undefined}>Normal</button>
                            <button onClick={bright_brightness_select} className={brightness === brightness_modes.bright ? device_page_styles.option_selected : undefined}>Bright</button>
                        </div>
                    </div>
                    <div>
                        <p>
                            Primary Color
                        </p>
                        <div className={device_page_styles.color_options}>
                            <button onClick={red_primary_color_select} className={((r1 === 255) && (g1 === 0) && (b1 === 0)) ? `${device_page_styles.red_color_option} ${device_page_styles.color_selected}` : device_page_styles.red_color_option } />
                            <button onClick={orange_primary_color_select} className={((r1 === 255) && (g1 === 190) && (b1 === 0)) ? `${device_page_styles.orange_color_option} ${device_page_styles.color_selected}` : device_page_styles.orange_color_option } />
                            <button onClick={yellow_primary_color_select} className={((r1 === 255) && (g1 === 255) && (b1 === 0)) ? `${device_page_styles.yellow_color_option} ${device_page_styles.color_selected}` : device_page_styles.yellow_color_option } />
                            <button onClick={blue_primary_color_select} className={((r1 === 0) && (g1 === 0) && (b1 === 255)) ? `${device_page_styles.blue_color_option} ${device_page_styles.color_selected}` : device_page_styles.blue_color_option } />
                            <button onClick={white_primary_color_select} className={((r1 === 255) && (g1 === 255) && (b1 === 255)) ? `${device_page_styles.white_color_option} ${device_page_styles.color_selected}` : device_page_styles.white_color_option } />
                        </div>
                    </div>
                    { mode == modes.alternate ? 
                        <div>
                            <p>
                                Seccondary Color
                            </p>
                            <div className={device_page_styles.color_options}>
                                <button onClick={red_seccondary_color_select} className={((r2 === 255) && (g2 === 0) && (b2 === 0)) ? `${device_page_styles.red_color_option} ${device_page_styles.color_selected}` : device_page_styles.red_color_option } />
                                <button onClick={orange_seccondary_color_select} className={((r2 === 255) && (g2 === 190) && (b2 === 0)) ? `${device_page_styles.orange_color_option} ${device_page_styles.color_selected}` : device_page_styles.orange_color_option }/>
                                <button onClick={yellow_seccondary_color_select} className={((r2 === 255) && (g2 === 255) && (b2 === 0)) ? `${device_page_styles.yellow_color_option} ${device_page_styles.color_selected}` : device_page_styles.yellow_color_option } />
                                <button onClick={blue_seccondary_color_select} className={((r2 === 0) && (g2 === 0) && (b2 === 255)) ? `${device_page_styles.blue_color_option} ${device_page_styles.color_selected}` : device_page_styles.blue_color_option } />
                                <button onClick={white_seccondary_color_select} className={((r2 === 255) && (g2 === 255) && (b2 === 255)) ? `${device_page_styles.white_color_option} ${device_page_styles.color_selected}` : device_page_styles.white_color_option } />
                            </div>
                        </div>
                    : undefined }
                    <div>
                        <p>
                            Device Active Enable
                        </p>
                        <div>
                            <button className={active === 1 ? device_page_styles.option_selected : undefined} onClick={enable_active_select}>
                                Enable
                            </button>
                            <button className={active === 0 ? device_page_styles.option_selected : undefined} onClick={disable_active_select}>
                                Disable
                            </button>
                        </div>
                    </div>
                    <div className={device_page_styles.mode_options}>
                        <p> Device Mode </p>
                        <div>
                            <button onClick={still_mode_select} className={mode === modes.still ? device_page_styles.option_selected : undefined}>
                                Still
                            </button>
                            <button onClick={flash_mode_select} className={mode === modes.flash ? device_page_styles.option_selected : undefined}>
                                Flash
                            </button>
                            <button onClick={alternate_mode_select} className={mode === modes.alternate ? device_page_styles.option_selected : undefined}>
                                Alternate
                            </button>
                        </div>
                    </div>
                    <div className={device_page_styles.device_options}>
                        <p>
                            Configuration Options
                        </p>
                        <div>
                            <button onClick={clear_car_count_click}>Clear Car Count</button>
                            <button onClick={update_config_click}>Update Configuration</button>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    )

}