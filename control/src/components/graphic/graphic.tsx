import { ReactNode } from "react";
import graphic_styles from "./graphic.module.css"; 

type graphic_props = {
    active: Number;
    color: string;
    className?: string;
}

export default function Graphic(props: graphic_props) {

    const show_light_glow = () => {

        let enabled_class = {
            backgroundColor: props.color,
            boxShadow: `0 0 15px 1px ${props.color}`
        }
        let disabled_class = {
            backgroundColor: "rgba(0,0,0,0)",
            border: `2px dotted ${props.color}`,
            boxShadow: `0 0 15px 2px ${props.color}`
        }
        
        return props.active == 1 ? enabled_class : disabled_class

    }

    return (
        <div className={`${graphic_styles.graphic} ${props.className}`} style={show_light_glow()} />
    )

}