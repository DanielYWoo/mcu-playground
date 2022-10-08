width=39;
height=19;
depth=6;

union() {       
    // display walls
    translate([-2,-2,0]) cube([2+width+2, 2, depth]);
    translate([-2,height,0]) cube([2+width+2, 2, depth]);
    
    translate([-2,-2,0]) cube([2, height+2, depth]);
    translate([width,-2,0]) cube([2, height+2, depth]);
}


