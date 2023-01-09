/*
 *   Copyright (c) 2023 Malte Hering
 *   All rights reserved.

 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *   http://www.apache.org/licenses/LICENSE-2.0

 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

// Helper
function getOffset(el) {
    var rect = el.getBoundingClientRect();
    return {
        left: rect.left + window.pageXOffset,
        top: rect.top + window.pageYOffset,
        width: rect.width || el.offsetWidth,
        height: rect.height || el.offsetHeight
    };
}
function clearConnectDivs() {
    let olddiv = document.getElementById("LineDiv");
    if (olddiv) {
        olddiv.remove();
        olddiv = null;
    }
}
function connectDivs(div1, div2, color, thickness) { // draw a line connecting elements
    var off1 = getOffset(div1);
    var off2 = getOffset(div2);
    // bottom right
    var x1 = off1.left + off1.width / 2;
    var y1 = off1.top + off1.height / 2;
    // top right
    var x2 = off2.left + off2.width / 2;
    var y2 = off2.top + off2.height / 2;
    // distance
    var length = Math.sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
    // center
    var cx = ((x1 + x2) / 2) - (length / 2);
    var cy = ((y1 + y2) / 2) - (thickness / 2);
    // angle
    var angle = Math.atan2((y1 - y2), (x1 - x2)) * (180 / Math.PI);
    // make hr

    clearConnectDivs();

    var div = document.createElement("div");
    div.style.padding = 0;
    div.style.margin = 0;
    div.style.height = thickness + "px";
    div.style.background = color;
    div.style.lineHeight = "1px";
    div.style.position = "absolute";
    div.style.left = cx + "px";
    div.style.top = cy + "px";
    div.style.width = length + "px";
    div.style.transform = "rotate(" + angle + "deg)";
    div.style.pointerEvents = "none"
    div.id = "LineDiv";

    // alert(htmlLine);
    document.body.appendChild(div);


}

// Multie

class Image {
    constructor() {
        this.width = 1;
        this.height = 1;
        this.pixels = [];
    }

    drawImage() {
        let editorDiv = document.getElementById("imageEditor");
        for (let a = 0; a < editorDiv.children.length; a++) {
            let x = parseInt(editorDiv.children[a].getAttribute("x"));
            let y = parseInt(editorDiv.children[a].getAttribute("y"));
            if (image.pixels[x][y]) {
                editorDiv.children[a].style.background = "black";
            }
            else {
                editorDiv.children[a].style.background = "white";
            }
        }
    }

    getButton(x, y) {
        return document.querySelector(`button[x="${x}"][y="${y}"]`);

    }
};



let image = new Image();

let oldTool = "";
let tools = {
    drawpixel: {
        click: (tool, event, button, x, y) => {
            if (event.type == "mousedown") {
                if (image.pixels[x][y] == true) {
                    image.pixels[x][y] = false;
                }
                else {
                    image.pixels[x][y] = true;
                }
            }
            else if (event.type == "mouseenter") {
                if (event.buttons == 1) {
                    image.pixels[x][y] = true;
                }
                else if (event.buttons == 2) {
                    image.pixels[x][y] = false;
                }
            }
            if (image.pixels[x][y] == true) {
                button.style.background = "black";
            }
            else {
                button.style.background = "white";
            }
            return true;
        }
    },
    line: {
        click: (tool, event, button, x, y) => {
            if (event.type == "mousedown" || event.type == "manuel") {
                if (event.buttons == 1) {
                    tool.start.x = x;
                    tool.start.y = y;
                    document.getElementById("lineStartX").value = tool.start.x;
                    document.getElementById("lineStartY").value = tool.start.y;
                }
                else if (event.buttons == 2) {
                    tool.end.x = x;
                    tool.end.y = y;
                    document.getElementById("lineEndX").value = tool.end.x;
                    document.getElementById("lineEndY").value = tool.end.y;
                }

                image.drawImage();

                if (tool.start.x != null && tool.start.y != null && tool.end.x != null && tool.end.y != null) {

                    let dirX = tool.start.x - tool.end.x;
                    let dirY = tool.start.y - tool.end.y;

                    let orgX = tool.end.x;
                    let orgY = tool.end.y;

                    let length = Math.sqrt(Math.pow(dirX, 2) + Math.pow(dirY, 2));
                    let step = 1 / length;

                    for (let a = 0; a < 1; a += step) {

                        let posX = orgX + dirX * a;
                        let posY = orgY + dirY * a;
                        let Xindex = Math.round(posX);
                        let Yindex = Math.round(posY);
                        //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            image.getButton(Xindex, Yindex).style.background = "lime";
                        }

                    }

                }
                if (tool.start.x != null && tool.start.y != null) {
                    let startButton = image.getButton(tool.start.x, tool.start.y);
                    startButton.style.background = "orange";
                }
                if (tool.end.x != null && tool.end.y != null) {
                    let endButton = image.getButton(tool.end.x, tool.end.y);
                    endButton.style.background = "green";
                }

                /*  let startbutton = document.querySelector(`button[x="${tool.start.x}"][y="${tool.start.y}"]`)
                  let endbutton = document.querySelector(`button[x="${tool.end.x}"][y="${tool.end.y}"]`)
                  connectDivs(startbutton, endbutton, "black", 10)
                  */


            }

            return true;
        },
        start: {
            x: null,
            y: null
        },
        end: {
            x: null,
            y: null
        },
        line: null,
        draw(tool) {

            let dirX = tool.start.x - tool.end.x;
            let dirY = tool.start.y - tool.end.y;

            let orgX = tool.end.x;
            let orgY = tool.end.y;

            let length = Math.sqrt(Math.pow(dirX, 2) + Math.pow(dirY, 2));
            let step = 1 / length;

            for (let a = 0; a < 1; a += step) {
                let posX = orgX + dirX * a;
                let posY = orgY + dirY * a;
                let Xindex = Math.round(posX);
                let Yindex = Math.round(posY);
                //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                    image.pixels[Xindex][Yindex] = true;
                }
            }
            image.drawImage();
            if (tool.start.x != null && tool.start.y) {
                let startButton = image.getButton(tool.start.x, tool.start.y);
                startButton.style.background = "orange";
            }
            if (tool.end.x != null && tool.end.y) {
                let endButton = image.getButton(tool.end.x, tool.end.y);
                endButton.style.background = "green";
            }
        }
    },
    circle: {
        click: (tool, event, button, x, y) => {
            if (event.type == "mousedown" || event.type == "manuel") {
                if (event.buttons == 1) {
                    tool.center.x = x;
                    tool.center.y = y;
                    document.getElementById("circleCenterX").value = tool.center.x;
                    document.getElementById("circleCenterY").value = tool.center.y;
                }

                image.drawImage();

                if (tool.center.x != null && tool.center.y != null) {
                    tool.radius = document.getElementById("circleRadius").value;


                    let r = tool.radius
                    let length = 2 * Math.PI * r;
                    let step = 1 / length;

                    for (let a = 0; a < (2 * Math.PI); a += step) {
                        let x = Math.sin(a) * r + tool.center.x;
                        let y = Math.cos(a) * r + tool.center.y;
                        let Xindex = Math.round(x);
                        let Yindex = Math.round(y);
                        //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            if (tool.fill) {
                                image.getButton(Xindex, Yindex).style.background = "lime";
                            }
                            else {
                                image.getButton(Xindex, Yindex).style.background = "blue";
                            }


                        }
                    }




                }


                if (tool.center.x != null && tool.center.y) {
                    let startButton = image.getButton(tool.center.x, tool.center.y);
                    startButton.style.background = "blue";
                }

                /*if (tool.start.x != null && tool.start.y && tool.end.x != null && tool.end.y != null) {
                    let startbutton = document.querySelector(`button[x="${tool.start.x}"][y="${tool.start.y}"]`)
                    let endbutton = document.querySelector(`button[x="${tool.end.x}"][y="${tool.end.y}"]`)
                    connectDivs(startbutton, endbutton, "black", 10)
                }*/

            }

            return true;
        },
        draw(tool) {
            tool.radius = document.getElementById("circleRadius").value;
            let fillStart = 0.1;
            let fillStep = 1 / tool.radius;
            if (!tool.fill) {
                fillStart = tool.radius;

            }
            for (let r = fillStart; r <= tool.radius; r += fillStep) {

                let length = 2 * Math.PI * r;
                let step = 1 / length;

                for (let a = 0; a < (2 * Math.PI); a += step) {
                    let x = Math.sin(a) * r + tool.center.x;
                    let y = Math.cos(a) * r + tool.center.y;
                    let Xindex = Math.round(x);
                    let Yindex = Math.round(y);
                    //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                    if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                        image.pixels[Xindex][Yindex] = true;
                    }
                }
            }
            image.drawImage();
        },

        center: {
            x: null,
            y: null
        },
        radius: null,
        fill: false,

    },
    rectangle: {
        click: (tool, event, button, x, y) => {
            if (event.type == "mousedown" || event.type == "manuel") {
                if (event.buttons == 1) {
                    tool.start.x = x;
                    tool.start.y = y;
                    document.getElementById("rectangleStartX").value = tool.start.x;
                    document.getElementById("rectangleStartY").value = tool.start.y;
                }
                else if (event.buttons == 2) {
                    tool.end.x = x;
                    tool.end.y = y;
                    document.getElementById("rectangleEndX").value = tool.end.x;
                    document.getElementById("rectangleEndY").value = tool.end.y;
                }

                image.drawImage();

                if (tool.start.x != null && tool.start.y != null && tool.end.x != null && tool.end.y != null) {

                    let left = Math.min(tool.start.x, tool.end.x);
                    let right = Math.max(tool.start.x, tool.end.x);

                    let top = Math.min(tool.start.y, tool.end.y);
                    let bottom = Math.max(tool.start.y, tool.end.y);


                    let step = 1 / (right - left);
                    for (let x = left; x <= right; x += step) {
                        let Xindex = Math.round(x);
                        // Top
                        let Yindex = Math.round(top);
                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            if (tool.fill) {
                                image.getButton(Xindex, Yindex).style.background = "lime";
                            }
                            else {
                                image.getButton(Xindex, Yindex).style.background = "blue";
                            }
                        }
                        // bottom
                        Yindex = Math.round(bottom);
                        //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            if (tool.fill) {
                                image.getButton(Xindex, Yindex).style.background = "lime";
                            }
                            else {
                                image.getButton(Xindex, Yindex).style.background = "blue";
                            }
                        }
                    }
                    step = 1 / (bottom - top);
                    for (let y = top; y <= bottom; y += step) {
                        let Yindex = Math.round(y);
                        let Xindex = Math.round(right);
                        // right

                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            if (tool.fill) {
                                image.getButton(Xindex, Yindex).style.background = "lime";
                            }
                            else {
                                image.getButton(Xindex, Yindex).style.background = "blue";
                            }
                        }
                        // left
                        Xindex = Math.round(left);
                        //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            if (tool.fill) {
                                image.getButton(Xindex, Yindex).style.background = "lime";
                            }
                            else {
                                image.getButton(Xindex, Yindex).style.background = "blue";
                            }
                        }
                    }
                }
                if (tool.start.x != null && tool.start.y != null) {
                    let startButton = image.getButton(tool.start.x, tool.start.y);
                    startButton.style.background = "orange";
                }
                if (tool.end.x != null && tool.end.y != null) {
                    let endButton = image.getButton(tool.end.x, tool.end.y);
                    endButton.style.background = "green";
                }

                /*  let startbutton = document.querySelector(`button[x="${tool.start.x}"][y="${tool.start.y}"]`)
                  let endbutton = document.querySelector(`button[x="${tool.end.x}"][y="${tool.end.y}"]`)
                  connectDivs(startbutton, endbutton, "black", 10)
                  */


            }

            return true;
        },
        start: {
            x: null,
            y: null
        },
        end: {
            x: null,
            y: null
        },
        fill: false,
        draw(tool) {

            tool.fill = document.getElementById("rectangleFill").checked;

            let left = Math.min(tool.start.x, tool.end.x);
            let right = Math.max(tool.start.x, tool.end.x);

            let top = Math.min(tool.start.y, tool.end.y);
            let bottom = Math.max(tool.start.y, tool.end.y);

            if (tool.fill) {
                let stepX = 1 / (right - left);
                let stepY = 1 / (bottom - top);
                for (let x = left; x <= right; x += stepX) {
                    for (let y = top; y <= bottom; y += stepY) {
                        let Xindex = Math.round(x);
                        let Yindex = Math.round(y);
                        // Top
                        if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                            image.pixels[Xindex][Yindex] = true;
                        }
                    
                    }
                }
            }
            else {


                let step = 1 / (right - left);
                for (let x = left; x <= right; x += step) {
                    let Xindex = Math.round(x);
                    // Top
                    let Yindex = Math.round(top);
                    if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                        image.pixels[Xindex][Yindex] = true;
                    }
                    // bottom
                    Yindex = Math.round(bottom);
                    //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                    if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                        image.pixels[Xindex][Yindex] = true;
                    }
                }
                step = 1 / (bottom - top);
                for (let y = top; y <= bottom; y += step) {
                    let Yindex = Math.round(y);
                    let Xindex = Math.round(right);
                    // right

                    if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                        image.pixels[Xindex][Yindex] = true;
                    }
                    // left
                    Xindex = Math.round(left);
                    //console.log("a", a, "length", length, "step", step, "Xindex", Xindex, "Yindex", Yindex);
                    if (Xindex >= 0 && Xindex < image.width && Yindex >= 0 && Yindex < image.height) {
                        image.pixels[Xindex][Yindex] = true;
                    }
                }
            }
            image.drawImage();
            if (tool.start.x != null && tool.start.y) {
                let startButton = image.getButton(tool.start.x, tool.start.y);
                startButton.style.background = "orange";
            }
            if (tool.end.x != null && tool.end.y) {
                let endButton = image.getButton(tool.end.x, tool.end.y);
                endButton.style.background = "green";
            }
        }
    },
}

function clickPixel(event) {
    /**
     * type HTMLButtonElement
     */
    console.log("Event");
    let button = null;
    let x = 0;
    let y = 0;
    if (event) {
        event.preventDefault();
        button = event.target;
        x = parseInt(button.getAttribute("x"));
        y = parseInt(button.getAttribute("y"));
    }
    else {
        event = {};
        event.type == "mousedown";
    }

    // Check Tooltype
    var radioButtons = document.getElementsByName("tool");
    let tool = "";
    for (let a = 0; a < radioButtons.length; a++) {

        if (radioButtons[a].checked == true) {
            tool = radioButtons[a].value;
            break;
        }
    }

    let keys = Object.keys(tools);
    let result = true;
    if (keys.includes(tool)) {
        if (tool != oldTool) {
            clearConnectDivs();
            image.drawImage();
            oldTool = tool;
        }
        result = tools[tool].click(tools[tool], event, button, x, y);
    }

    createImageBin();

    return result;
}

function preventContectmenu(event) {
    event.preventDefault();
}

function createImageEditor() {
    let widthinput = document.getElementById("inputwidth");
    let inputheight = document.getElementById("inputheight");
    image.width = parseInt(widthinput.value);
    image.height = parseInt(inputheight.value);

    image.pixels = [];
    for (let x = 0; x < image.width; x++) {
        let arr = [];
        for (let y = 0; y < image.height; y++) {
            arr.push(false);
        }
        image.pixels.push(arr);
    }

    console.log(image);

    let editorDiv = document.getElementById("imageEditor");
    editorDiv.innerHTML = "";

    //editorDiv.style.height = Math.min(editorDiv.clientHeight,editorDiv.clientWidth) + "px";

    for (let x = 0; x < image.width; x++) {
        for (let y = 0; y < image.height; y++) {
            let button = document.createElement("button");
            button.classList.add("imageEditorButton");
            button.style.background = "white";
            button.style.left = (100 / image.width * x) + "%";
            button.style.top = (100 / image.height * y) + "%";

            button.style.width = (100 / image.width) + "%";
            button.style.height = (100 / image.height) + "%";
            button.style.padding = "0";
            button.style.margin = "0";

            button.setAttribute("x", x);
            button.setAttribute("y", y);


            button.onmousedown = clickPixel;
            button.onmouseenter = clickPixel;
            button.oncontextmenu = preventContectmenu;
            editorDiv.appendChild(button);
        }
    }

    createImageBin();
}

function createImageBin() {

    let byteArray = [];
    for (let y = 0; y < image.height; y++) {
        let bytes = [0];
        let bytePos = 0;
        let index = 0;
        for (let x = 0; x < image.width; x++) {
            if (bytePos > 7) {
                bytePos = 0;
                bytes.push(0);
                index++;
            }
            if (image.pixels[x][y]) {
                bytes[index] |= 1 << (7 - bytePos);
            }
            bytePos++;
        }
        byteArray.push(bytes);
    }


    let text = "";
    for (let a = 0; a < byteArray.length; a++) {
        let line = byteArray[a].map((byte) => {
            return "0x" + byte.toString(16).padStart(2, "0");
        }).join(", ");
        text += line + ",\n";
    }


    /*let index = 0;
    let bytePos = 0;
    for (let x = 0; x < image.width; x++) {
        for (let y = 0; y < image.height; y++) {
            if (bytePos > 7) {
                bytePos = 0;
                byteArray.push(0);
                index++;
            }
            if (image.pixels[x][y]) {
                byteArray[index] |= 1 << (7 - bytePos);
            }
            bytePos++;

        }
    }*/

    /*
        let text = "{\n";
        text += " ".repeat(3);
        for (let a = 0; a < byteArray.length; a++) {
            if (a % (image.width / 8) == 0 && a > 0) {
                text += "\n";
                text += " ".repeat(3);
            }
            text += "0x" + byteArray[a].toString(16).padStart(2, "0") + ", ";
    
        }
        text = text.substring(0, text.length - 2);
        text += "\n}";*/
    let textarea = document.getElementById("imageHex");
    textarea.value = text;

}

function createImagefromBin(image, text) {
    // Remove comments
    let lines = text.split("\n");
    let byteArray = lines.map((line) => {
        let index = line.indexOf("//");
        if (index >= 0) {
            line = line.substring(0, index);
        }
        line = line.replace(/[ \t]/g, "");
        let bytes = line.split(",").filter((byte) => {
            return byte.length > 0;
        });
        return bytes;
    })

    console.log(byteArray);

    for (let y = 0; y < image.height; y++) {
        let index = 0;
        let bytePos = 0;
        for (let x = 0; x < image.width; x++) {
            if (bytePos > 7) {
                bytePos = 0;

                index++;
            }
            console.log("Pixel x", x, "y", y, "byte", byteArray[y][index])
            if (byteArray[y][index] & 1 << (7 - bytePos)) {
                image.pixels[x][y] = true;
            }
            else {
                image.pixels[x][y] = false;
            }
            bytePos++;
        }
    }
    // Set Editor
    image.drawImage();

}

function onChangeImageBinTexArea() {
    let textarea = document.getElementById("imageHex");
    createImagefromBin(image, textarea.value);
}



window.onload = () => {
    createImageEditor();

}







