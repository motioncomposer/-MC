{
    "patcher" :     {
        "fileversion" : 1,
        "appversion" :         {
            "major" : 5,
            "minor" : 1,
            "revision" : 9
        }
,
        "rect" : [ 71.0, 83.0, 937.0, 548.0 ],
        "bglocked" : 0,
        "defrect" : [ 71.0, 83.0, 937.0, 548.0 ],
        "openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
        "openinpresentation" : 0,
        "default_fontsize" : 12.0,
        "default_fontface" : 0,
        "default_fontname" : "Arial",
        "gridonopen" : 0,
        "gridsize" : [ 15.0, 15.0 ],
        "gridsnaponopen" : 0,
        "toolbarvisible" : 1,
        "boxanimatetime" : 200,
        "imprint" : 0,
        "enablehscroll" : 1,
        "enablevscroll" : 1,
        "devicewidth" : 0.0,
        "boxes" : [             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "start",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 676.0, 151.0, 34.0, 18.0 ],
                    "id" : "obj-49",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "dac~",
                    "numinlets" : 2,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 750.0, 208.0, 37.0, 20.0 ],
                    "id" : "obj-48",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "stop",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 563.0, 173.0, 33.0, 18.0 ],
                    "id" : "obj-47",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "delay 100",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "bang" ],
                    "patching_rect" : [ 563.0, 148.0, 63.0, 20.0 ],
                    "id" : "obj-46",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "840",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 828.0, 94.0, 32.5, 18.0 ],
                    "id" : "obj-45",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "640",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 699.0, 94.0, 32.5, 18.0 ],
                    "id" : "obj-44",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "440",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 580.0, 95.0, 32.5, 18.0 ],
                    "id" : "obj-43",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "OSC-route /zone",
                    "numinlets" : 1,
                    "numoutlets" : 2,
                    "fontsize" : 12.0,
                    "outlettype" : [ "", "" ],
                    "patching_rect" : [ 812.0, 64.0, 101.0, 20.0 ],
                    "id" : "obj-41",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "OSC-route /player/2",
                    "numinlets" : 1,
                    "numoutlets" : 2,
                    "fontsize" : 12.0,
                    "outlettype" : [ "", "" ],
                    "patching_rect" : [ 684.0, 62.0, 117.0, 20.0 ],
                    "id" : "obj-39",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "cycle~",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "signal" ],
                    "patching_rect" : [ 756.0, 152.0, 45.0, 20.0 ],
                    "id" : "obj-29",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "OSC-route /player/1",
                    "numinlets" : 1,
                    "numoutlets" : 2,
                    "fontsize" : 12.0,
                    "outlettype" : [ "", "" ],
                    "patching_rect" : [ 563.0, 64.0, 117.0, 20.0 ],
                    "id" : "obj-18",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "print MEtoTM",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 610.0, 410.0, 83.0, 20.0 ],
                    "id" : "obj-17",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "print MEtoCM",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 123.0, 432.0, 84.0, 20.0 ],
                    "id" : "obj-16",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "comment",
                    "text" : " messages to TM",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 697.0, 441.0, 111.0, 20.0 ],
                    "id" : "obj-15",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "comment",
                    "text" : " messages to CM",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 238.0, 439.0, 111.0, 20.0 ],
                    "id" : "obj-12",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/alphabet/player/1/activity/discrete 1",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 672.0, 232.0, 221.0, 18.0 ],
                    "id" : "obj-11",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "udpsend 127.0.0.1 6061",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 591.0, 479.0, 140.0, 20.0 ],
                    "id" : "obj-10",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/alphabet/player/activity/normal 1",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 651.0, 255.0, 205.0, 18.0 ],
                    "id" : "obj-9",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "print MEfrTM",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 717.0, 33.0, 80.0, 20.0 ],
                    "id" : "obj-7",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "udpreceive 6160",
                    "numinlets" : 1,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 563.0, 6.0, 99.0, 20.0 ],
                    "id" : "obj-8",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "comment",
                    "text" : "Musical\nEnvironment\n(ME)\nstarted\n:)",
                    "linecount" : 5,
                    "presentation_linecount" : 5,
                    "frgb" : [ 1.0, 1.0, 1.0, 1.0 ],
                    "numinlets" : 1,
                    "presentation_rect" : [ 4.0, 7.0, 119.0, 110.0 ],
                    "numoutlets" : 0,
                    "fontface" : 1,
                    "fontsize" : 18.0,
                    "patching_rect" : [ 11.0, 12.0, 119.0, 110.0 ],
                    "presentation" : 1,
                    "textcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
                    "id" : "obj-5",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "panel",
                    "numinlets" : 1,
                    "presentation_rect" : [ 0.0, 0.0, 128.0, 128.0 ],
                    "numoutlets" : 0,
                    "patching_rect" : [ 7.0, 5.0, 128.0, 128.0 ],
                    "presentation" : 1,
                    "bgcolor" : [ 0.478431, 0.709804, 0.317647, 1.0 ],
                    "id" : "obj-2"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "button",
                    "numinlets" : 1,
                    "numoutlets" : 1,
                    "outlettype" : [ "bang" ],
                    "patching_rect" : [ 340.0, 144.0, 20.0, 20.0 ],
                    "id" : "obj-38"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "gate 9",
                    "numinlets" : 2,
                    "numoutlets" : 9,
                    "fontsize" : 12.0,
                    "outlettype" : [ "", "", "", "", "", "", "", "", "" ],
                    "patching_rect" : [ 232.0, 171.0, 127.0, 20.0 ],
                    "id" : "obj-37",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "0",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 368.0, 87.0, 32.5, 19.0 ],
                    "id" : "obj-3",
                    "fontname" : "Lucida Grande"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "button",
                    "numinlets" : 1,
                    "numoutlets" : 1,
                    "outlettype" : [ "bang" ],
                    "patching_rect" : [ 340.0, 86.0, 20.0, 20.0 ],
                    "id" : "obj-4"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "toggle",
                    "numinlets" : 1,
                    "numoutlets" : 1,
                    "outlettype" : [ "int" ],
                    "patching_rect" : [ 340.0, 37.0, 20.0, 20.0 ],
                    "id" : "obj-34"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "metro 10",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "bang" ],
                    "patching_rect" : [ 340.0, 63.0, 64.0, 21.0 ],
                    "id" : "obj-35",
                    "fontname" : "Lucida Grande"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "counter 1 9",
                    "numinlets" : 5,
                    "numoutlets" : 4,
                    "fontsize" : 12.0,
                    "outlettype" : [ "int", "", "", "int" ],
                    "patching_rect" : [ 340.0, 108.0, 78.0, 21.0 ],
                    "id" : "obj-36",
                    "fontname" : "Lucida Grande"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "OSC-route /set/ME/initialize",
                    "numinlets" : 1,
                    "numoutlets" : 2,
                    "fontsize" : 12.0,
                    "outlettype" : [ "", "" ],
                    "patching_rect" : [ 149.0, 109.0, 158.0, 20.0 ],
                    "id" : "obj-33",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "loadbang",
                    "numinlets" : 1,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "bang" ],
                    "patching_rect" : [ 18.0, 150.0, 60.0, 20.0 ],
                    "id" : "obj-32",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/zone/2/soundbank/1/list piano violin viola flute",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 300.0, 264.0, 277.0, 18.0 ],
                    "id" : "obj-28",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/zone/1/soundbank/1/list piano violin viola flute",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 286.0, 287.0, 277.0, 18.0 ],
                    "id" : "obj-19",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "print MEfrCM",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 200.0, 50.0, 81.0, 20.0 ],
                    "id" : "obj-26",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "udpreceive 6560",
                    "numinlets" : 1,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 149.0, 17.0, 99.0, 20.0 ],
                    "id" : "obj-27",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/player/2/soundbank/2/list piano violin viola flute",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 273.0, 309.0, 283.0, 18.0 ],
                    "id" : "obj-25",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/player/2/soundbank/1/list piano violin viola flute",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 259.0, 332.0, 283.0, 18.0 ],
                    "id" : "obj-24",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/player/1/soundbank/2/list piano violin viola flute",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 246.0, 356.0, 283.0, 18.0 ],
                    "id" : "obj-23",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/player/1/soundbank/1/list piano violin viola flute",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 232.0, 379.0, 283.0, 18.0 ],
                    "id" : "obj-22",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/ME/ready",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 340.0, 198.0, 85.0, 18.0 ],
                    "id" : "obj-21",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "message",
                    "text" : "/set/ME/loaded",
                    "numinlets" : 2,
                    "numoutlets" : 1,
                    "fontsize" : 12.0,
                    "outlettype" : [ "" ],
                    "patching_rect" : [ 18.0, 185.0, 91.0, 18.0 ],
                    "id" : "obj-20",
                    "fontname" : "Arial"
                }

            }
,             {
                "box" :                 {
                    "maxclass" : "newobj",
                    "text" : "udpsend 127.0.0.1 6065",
                    "numinlets" : 1,
                    "numoutlets" : 0,
                    "fontsize" : 12.0,
                    "patching_rect" : [ 30.0, 490.0, 140.0, 20.0 ],
                    "id" : "obj-1",
                    "fontname" : "Arial"
                }

            }
 ],
        "lines" : [             {
                "patchline" :                 {
                    "source" : [ "obj-46", 0 ],
                    "destination" : [ "obj-47", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-41", 0 ],
                    "destination" : [ "obj-46", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 821.5, 142.0, 572.5, 142.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-18", 0 ],
                    "destination" : [ "obj-46", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-39", 0 ],
                    "destination" : [ "obj-46", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 693.5, 142.0, 572.5, 142.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-39", 0 ],
                    "destination" : [ "obj-44", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 693.5, 86.0, 708.5, 86.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-41", 0 ],
                    "destination" : [ "obj-45", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 821.5, 90.0, 837.5, 90.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-45", 0 ],
                    "destination" : [ "obj-49", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 837.5, 132.0, 685.5, 132.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-44", 0 ],
                    "destination" : [ "obj-49", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 708.5, 132.0, 685.5, 132.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-49", 0 ],
                    "destination" : [ "obj-48", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 685.5, 204.0, 759.5, 204.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-43", 0 ],
                    "destination" : [ "obj-49", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 589.5, 131.0, 685.5, 131.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-47", 0 ],
                    "destination" : [ "obj-48", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 572.5, 199.0, 759.5, 199.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-45", 0 ],
                    "destination" : [ "obj-29", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 837.5, 121.0, 765.5, 121.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-44", 0 ],
                    "destination" : [ "obj-29", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 708.5, 121.0, 765.5, 121.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-29", 0 ],
                    "destination" : [ "obj-48", 1 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-29", 0 ],
                    "destination" : [ "obj-48", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-43", 0 ],
                    "destination" : [ "obj-29", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 589.5, 121.0, 765.5, 121.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-18", 0 ],
                    "destination" : [ "obj-43", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 572.5, 89.0, 589.5, 89.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-8", 0 ],
                    "destination" : [ "obj-41", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 572.5, 58.0, 821.5, 58.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-8", 0 ],
                    "destination" : [ "obj-18", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-8", 0 ],
                    "destination" : [ "obj-39", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 572.5, 52.0, 693.5, 52.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-36", 2 ],
                    "destination" : [ "obj-34", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 388.833344, 140.0, 502.0, 140.0, 502.0, 7.0, 349.5, 7.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-3", 0 ],
                    "destination" : [ "obj-36", 2 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-4", 0 ],
                    "destination" : [ "obj-36", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-36", 0 ],
                    "destination" : [ "obj-38", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-36", 0 ],
                    "destination" : [ "obj-37", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 349.5, 140.0, 321.0, 140.0, 321.0, 158.0, 241.5, 158.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-9", 0 ],
                    "destination" : [ "obj-17", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 660.5, 386.0, 619.5, 386.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-11", 0 ],
                    "destination" : [ "obj-17", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 681.5, 252.0, 619.5, 252.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-28", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 309.5, 282.0, 132.5, 282.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-19", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 295.5, 306.0, 132.5, 306.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-25", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 282.5, 327.0, 132.5, 327.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-24", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 268.5, 351.0, 132.5, 351.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-23", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 255.5, 375.0, 132.5, 375.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-22", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 241.5, 417.0, 132.5, 417.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-21", 0 ],
                    "destination" : [ "obj-16", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 349.5, 249.0, 132.5, 249.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-8", 0 ],
                    "destination" : [ "obj-7", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 572.5, 30.0, 726.5, 30.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-27", 0 ],
                    "destination" : [ "obj-26", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 158.5, 43.0, 209.5, 43.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 7 ],
                    "destination" : [ "obj-11", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 336.0, 231.0, 681.5, 231.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-11", 0 ],
                    "destination" : [ "obj-10", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 681.5, 252.0, 600.5, 252.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-9", 0 ],
                    "destination" : [ "obj-10", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 660.5, 399.0, 600.5, 399.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 6 ],
                    "destination" : [ "obj-9", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 322.5, 243.0, 660.5, 243.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 8 ],
                    "destination" : [ "obj-21", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 5 ],
                    "destination" : [ "obj-28", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 4 ],
                    "destination" : [ "obj-19", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 3 ],
                    "destination" : [ "obj-25", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 2 ],
                    "destination" : [ "obj-24", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 1 ],
                    "destination" : [ "obj-23", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-37", 0 ],
                    "destination" : [ "obj-22", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-38", 0 ],
                    "destination" : [ "obj-37", 1 ],
                    "hidden" : 0,
                    "midpoints" : [ 349.5, 167.0, 349.5, 167.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-32", 0 ],
                    "destination" : [ "obj-20", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-34", 0 ],
                    "destination" : [ "obj-35", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-35", 0 ],
                    "destination" : [ "obj-4", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-27", 0 ],
                    "destination" : [ "obj-33", 0 ],
                    "hidden" : 0,
                    "midpoints" : [  ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-33", 0 ],
                    "destination" : [ "obj-34", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 158.5, 140.0, 312.0, 140.0, 312.0, 7.0, 349.5, 7.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-22", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 241.5, 408.0, 39.5, 408.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-23", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 255.5, 372.0, 39.5, 372.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-24", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 268.5, 348.0, 39.5, 348.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-25", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 282.5, 324.0, 39.5, 324.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-19", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 295.5, 303.0, 39.5, 303.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-28", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 309.5, 279.0, 39.5, 279.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-20", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 27.5, 405.0, 39.5, 405.0 ]
                }

            }
,             {
                "patchline" :                 {
                    "source" : [ "obj-21", 0 ],
                    "destination" : [ "obj-1", 0 ],
                    "hidden" : 0,
                    "midpoints" : [ 349.5, 224.0, 39.5, 224.0 ]
                }

            }
 ]
    }

}
