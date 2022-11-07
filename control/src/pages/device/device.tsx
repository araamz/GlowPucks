import device_page_styles from "./device.module.css";

import { useNavigate, useParams } from "react-router-dom"
import DevicePreview from "../../components/device_preview/device_preview";
import ButtonWidget from "../../components/button_widget/button_widget";
import Graphic from "../../components/graphic/graphic";
import { useEffect, useState } from "react";
import LabelField from "../../components/label_field/label_field";
import Widget from "../../components/widget/widget";

enum modes {
    none = 1,
    flash = 2,
    alternate = 3
}

type device = {
    label: string;
    uuid: string;
    brightness: number;
    color: string;
    active: boolean;
    mode: modes;

}

const colors = [
    { color: "#f39c12" },
    { color: "#e74c3c" },
    { color: "#f1c40f" },
    { color: "#2980b9" },
    { color: "#2ecc71" },
    { color: "#eeeeee" }
]

export default function Device() {

    const [active, set_active] = useState<boolean>(true)
    const [mode, set_mode] = useState<modes>(modes.none)

    const [device_information, set_device_information] = useState<device>({
        label: "Glowpuck 1",
        uuid: "bbd549ca-5e44-11ed-9b6a-0242ac120002",
        brightness: 0,
        color: "none",
        active: false,
        mode: modes.none
    })
    const [device_configuration, set_device_configuration] = useState<device>({
        label: "",
        uuid: "",
        brightness: 0,
        color: "none",
        active: false,
        mode: modes.none
    })

    const handleColor = (event: any) => {
        let color = event.target.value

        console.log(color)
    }

    useEffect(() => {
        console.log(device_configuration)
    }, [device_configuration])

    let { device_id } = useParams()

    const handleActive = () => {

        if (active) {
            set_active(false)
        } else {
            set_active(true)
        }

    }

    const handleMode = (event: any) => {
        console.log(event.target.value)
        set_mode(Number(event.target.value))

    }

    return (
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
                            Color
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
        </div>
    )

}