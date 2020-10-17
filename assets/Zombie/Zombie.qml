import QtQuick 2.15
import QtQuick3D 1.15
import QtQuick.Timeline 1.0

Node {
    id: rootNode

    Model {
        id: z_arm_r
        x: -150
        y: 550
        z: -23.1576
        eulerRotation.x: -90
        scale.x: 100
        scale.y: 100
        scale.z: 100
        source: "meshes/z_arm_r.mesh"

        DefaultMaterial {
            id: defaultMaterial_material
            diffuseColor: "#ffcccccc"
        }
        materials: [
            defaultMaterial_material
        ]
    }

    Model {
        id: z_arm_l
        x: 150
        y: 550
        z: -23.1576
        eulerRotation.x: -90
        scale.x: 100
        scale.y: 100
        scale.z: 100
        source: "meshes/z_arm_l.mesh"
        materials: [
            defaultMaterial_material
        ]
    }

    Model {
        id: z_leg_l
        x: 50
        y: 150
        z: -23.1576
        eulerRotation.x: -90
        scale.x: 100
        scale.y: 100
        scale.z: 100
        source: "meshes/z_leg_l.mesh"
        materials: [
            defaultMaterial_material
        ]
    }

    Model {
        id: z_leg_r
        x: -50
        y: 150
        z: -23.1576
        eulerRotation.x: -90
        scale.x: 100
        scale.y: 100
        scale.z: 100
        source: "meshes/z_leg_r.mesh"
        materials: [
            defaultMaterial_material
        ]
    }

    Model {
        id: z_head
        x: 1.49012e-06
        y: 700
        z: -23.1576
        eulerRotation.x: -90
        scale.x: 100
        scale.y: 100
        scale.z: 100
        source: "meshes/z_head.mesh"
        materials: [
            defaultMaterial_material
        ]
    }

    Model {
        id: z_body
        x: 1.49012e-06
        y: 450
        z: -23.1576
        eulerRotation.x: -90
        scale.x: 100
        scale.y: 100
        scale.z: 100
        source: "meshes/z_body.mesh"
        materials: [
            defaultMaterial_material
        ]
    }

    Timeline {
        id: timeline0
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: true
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
            }
        ]
    }

    Timeline {
        id: timeline1
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: false
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
            }
        ]
    }

    Timeline {
        id: timeline2
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: false
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
            }
        ]
    }

    Timeline {
        id: timeline3
        startFrame: 0
        endFrame: 0
        currentFrame: 0
        enabled: false
        animations: [
            TimelineAnimation {
                duration: 0
                from: 0
                to: 0
                running: true
            }
        ]
    }
}
