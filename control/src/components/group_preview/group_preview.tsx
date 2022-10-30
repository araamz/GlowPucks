import { Key, useEffect, useState } from "react";
import Graphic from "../graphic/graphic";
import SettingsWidget from "../settings_widget/settings_widget";
import Widget from "../widget/widget";
import group_preview_styles from "./group_preview.module.css";

type group_preview_props = {
    id: any;
}

type device = {
    id: any;
    active: boolean;
    color: string;
}

export default function GroupPreview(props: group_preview_props) {

    const [extra_graphic_count, set_extra_graphic_count] = useState<Number>(0)
    const [shown_graphics, set_shown_graphics] = useState<device[] | null>()

    const click_handler = () => {
        console.log("Click from device " + "group")
        
    }

    const show_count = 3;

    const device_group = {
        label: "Artemisa Way/Virginia Street",
        devices: [
            {
                id: 0,
                active: true,
                color: "red"
            },
            {
                id: 1,
                active: false,
                color: "red"
            },
            {
                id: 2,
                active: false,
                color: "green"
            },
            {
                id: 3,
                active: true,
                color: "green"
            },
            {
                id: 4,
                active: true,
                color: "yellow"
            },
            {
                id: 5,
                active: true,
                color: "green"
            },
            {
                id: 6,
                active: true,
                color: "green"
            },
        ]
    }



    useEffect(() => {
        let graphic_count = device_group.devices.length;

        if (graphic_count > show_count) {
            set_extra_graphic_count(graphic_count - show_count)
        }

        set_shown_graphics(device_group.devices.slice(0,3))
    }, [])


    return (
        <div className={group_preview_styles.group_preview}>
            <SettingsWidget onClick={click_handler}/>
            <p className={group_preview_styles.label}>
                {device_group.label}
            </p>
            <div className={group_preview_styles.devices_container}>
                { shown_graphics?.map((graphic) => 
                    <Graphic className={group_preview_styles.graphic} key={graphic.id} active={graphic.active} color={graphic.color} /> 
                )}
            </div>
            { extra_graphic_count > 0 ?
            <Widget className={group_preview_styles.count_overflow}>
                <p>
                    +{extra_graphic_count}
                </p>
            </Widget> : null }
        </div>
    )

}