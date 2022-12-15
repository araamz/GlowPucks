import device_page_styles from "./device.module.css";

import { useNavigate, useParams } from "react-router-dom"
import DevicePreview from "../../components/device_preview/device_preview";
import ButtonWidget from "../../components/button_widget/button_widget";
import Graphic from "../../components/graphic/graphic";
import { useEffect, useState } from "react";
import Widget from "../../components/widget/widget";
import io, { Socket } from 'socket.io-client';

enum modes {
    still = 0,
    flash = 1,
    alternate = 2
}
enum protocol_message_type {
    config = 0,
    status = 1
}


const colors = [
    { color: "#f39c12" },
    { color: "#e74c3c" },
    { color: "#f1c40f" },
    { color: "#2980b9" },
    { color: "#2ecc71" },
    { color: "#eeeeee" }
]

//const socket = io("http://127.0.0.1:5000");

export default function Device() {

    let { device_uuid } = useParams();

    const [label, set_label] = useState<String>("Glowpuck")
    const [brightness, set_brightness] = useState<Number>(100)
    const [active, set_active] = useState<Number>(1)
    const [mode, set_mode] = useState<modes>(0)
    const [r1, set_r1] = useState<Number>(100)
    const [g1, set_g1] = useState<Number>(0)
    const [b1, set_b1] = useState<Number>(0)
    const [r2, set_r2] = useState<Number>()
    const [g2, set_g2] = useState<Number>()
    const [b2, set_b2] = useState<Number>()
    const [group_enable, set_group_enable] = useState<Number>(0)
    const [group_target, set_group_target] = useState<String>("NULL")
    const [car_count, set_car_count] = useState<Number>(0)

    const create_protocol_message = (type: protocol_message_type, car_clear: Number) => {

        if (car_clear == undefined) {
            car_clear = 0;
        }

        let protocol_message;

        if (type == protocol_message_type.config) {

            protocol_message = `${protocol_message_type.config}\t${label}\t${brightness}\t${active}\t${mode}\t${r1}\t${g1}\t${b1}\t${r2}\t${g2}\t${b2}\t${group_enable}\t${group_target}\t${car_clear}\t${-1}`;

        } else if (type == protocol_message_type.status) {

            protocol_message = `${protocol_message_type.status}\t${label}\t${brightness}\t${active}\t${mode}\t${r1}\t${g1}\t${b1}\t${r2}\t${g2}\t${b2}\t${group_enable}\t${group_target}\t${car_clear}\t${-1}`;

        }

        return protocol_message

    }

    const parse_protocol_status_message = (message: String) => {

        let tokens = message.split("\t")

        let parsed_type = Number(tokens[0])
        let parsed_label = tokens[1]
        let parsed_brightness = Number(tokens[2])
        let parsed_active = Number(tokens[3])
        let parsed_mode = Number(tokens[4])
        let parsed_r1 = Number(tokens[5])
        let parsed_g1 = Number(tokens[6])
        let parsed_b1 = Number(tokens[7])
        let parsed_r2 = Number(tokens[8])
        let parsed_g2 = Number(tokens[9])
        let parsed_b2 = Number(tokens[10])
        let parsed_group_enable = Number(tokens[11])
        let parsed_group_target = tokens[12]
        let parsed_car_count = Number(tokens[14])

        if (parsed_type == protocol_message_type.status) {
            return -1
        }

        set_label(parsed_label)
        set_brightness(parsed_brightness)
        set_active(parsed_active)
        set_mode(parsed_mode)
        set_r1(parsed_r1)
        set_g1(parsed_g1)
        set_b1(parsed_b1)
        set_r2(parsed_r2)
        set_g2(parsed_g2)
        set_b2(parsed_b2)
        set_group_enable(parsed_group_enable)
        set_group_target(parsed_group_target)
        set_car_count(parsed_car_count)

    }   

    const request_status_message = () => {


    }
    const transmit_config_message = (car_clear_enable: Number) => {

        let protocol_message = create_protocol_message(protocol_message_type.config, car_clear_enable)

        socket.emit("config_update", (device_uuid, protocol_message))

    }

    useEffect(() => {

        if (mode !== 2) {
            set_r2(-1);
            set_g2(-1);
            set_b2(-1);
        }

    }, [mode, set_r2, set_g2, set_b2])

    useEffect(() => {

        if (group_enable !== 1) {
            set_group_target("NULL")
        } 

    }, [group_enable, set_group_target])

    useEffect(() => {
        console.log(device_uuid)
    })

    useEffect(() => {

        let message = create_protocol_message(protocol_message_type.config, 0)
        parse_protocol_status_message(message)

    })

    return (

        <div>

        </div>

        /*
        <div className={device_page_styles.device_page}>
            <div className={device_page_styles.device_information}>
                <h5>Device Information</h5>
                <div className={device_page_styles.device_attributes}>
                    <div className={device_page_styles.graphic_attribute}>
                        <Graphic active={false} color={"red"} />
                    </div>
                    <div className={device_page_styles.label_attribute}>
                        <p>
                            Label
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>
                                { device_information?.label }
                            </p>
                        </div>
                    </div>
                    <div className={device_page_styles.uuid_attribute}>
                        <p>
                            UUID
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>{ device_information?.uuid }</p>
                        </div>
                    </div>
                    <div className={device_page_styles.brightness_attribute}>
                        <p>
                            Brightness
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>{ device_information?.brightness }</p>
                        </div>
                    </div>
                    <div className={device_page_styles.color_attribute}>
                        <p>
                            Primary Color
                        </p>
                        <div>   
                            { device_information?.color }
                        </div>
                    </div>
                    <div className={device_page_styles.active_attribute}>
                        <p>
                            Active
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>{ device_information?.active }</p>
                        </div>
                    </div>
                    <div className={device_page_styles.mode_attribute}>
                        <p>
                            Mode
                        </p>
                        <div>
                            <p className={device_page_styles.attribute_value}>{ device_information?.mode }</p>
                        </div>
                    </div>
                </div>
                <div>
                    <div>
                        <p>
                            Cars Seen
                        </p>
                        <p>
                            25
                        </p>
                    </div>
                    <Widget>
                        <span className={`material-symbols-outlined`}>
                            delete
                        </span>
                    </Widget>
                </div>
            </div>
            <div className={device_page_styles.device_configuration}>
                <h5>Device Configuration</h5>
                <div>
                    <div>
                        <p>
                            Change Device Label
                        </p>
                        <textarea />
                    </div>
                    <div>
                        <p>
                            Change Device Brightness
                        </p>
                    </div>
                    <div>
                        <p>
                            Change Device Color
                        </p>
                        <div>
                            { (mode === modes.none) || (mode === modes.flash) ? 
                            <div> { colors.map((color) => {
                                return (
                                    <button key={color.color} value={color.color} className={device_page_styles.color_box} style={{background: color.color}} onClick={handleColor} />
                                )
                            })} </div> :
                            <div> 
                                <div>
                                    <p>
                                        Primary Color
                                    </p>
                                    <div> 
                                        { colors.map((color) => {
                                            return (
                                                <button key={color.color} value={color.color} className={device_page_styles.color_box} style={{background: color.color}} onClick={handleColor} />
                                            )
                                        })} 
                                    </div>
                                </div>
                                <div>
                                    <p>
                                        Secondary Color
                                    </p>
                                    <div> 
                                        { colors.map((color) => {
                                            return (
                                                <button key={color.color} value={color.color} className={device_page_styles.color_box} style={{background: color.color}} onClick={handleColor} />
                                            )
                                        })} 
                                    </div>
                                </div>
                            </div> } 
                        </div>
                    </div>
                    <div>
                        <p>
                            Device Active Enable
                        </p>
                        <div>
                            { active ? <button onClick={handleActive}> Disable </button> : <button onClick={handleActive}> Enable</button>}
                        </div>
                    </div>
                    <div>
                        <p> Set Device Mode </p>
                        <div>
                            <div onChange={handleMode}>
                                <input defaultChecked={mode === modes.none} type={"radio"} value={modes.none} name={"mode"} /> None
                                <input defaultChecked={mode === modes.flash} type={"radio"} value={modes.flash} name={"mode"} /> Flash
                                <input defaultChecked={mode === modes.alternate} type={"radio"} value={modes.alternate} name={"mode"} /> Alternate
                            </div>
                        </div>
                    </div>
                    <button>Update Configuration</button>
                </div>
            </div>
        </div>*/
    )

}