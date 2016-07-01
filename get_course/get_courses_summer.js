main = frames['main'];
var xlx = 0, jxy = 0, ozwh = 0, skd = 0;
var total_cnt = 0;
var cnt = 0;

// function step1() { // find and click bixiu ke
//     bxk = main.document.getElementById("btnBxk");
//     bxk.click();
//     
//     console.log("step1 to step2");
//     setTimeout(step2, 1500);
// }

function step2() { // find and click the taget course, go to select page
    courses = main.document.getElementsByName("myradiogroup");
    for (var i = 0; i < courses.length; ++i) {
        if (courses[i].value == "PS909") { // jiji xinli xue
            courses[i].click();
            break;
        }
    }
    
    kcap = main.document.getElementById("lessonArrange");
    if (!kcap) setTimeout(step2, 1500);
    kcap.click();
    
    setTimeout(step3, 2000);
}

function step3() { // find taget teacher and judge whether is full
    teachers = main.document.getElementsByName("myradiogroup");
    tag_course = teachers[0];
    for (var i = 0; i < teachers.length; ++i) {
        if (teachers[i].value == 374585) {
            tag_course = teachers[i];
            break;
        }
    }
    
    if (!tag_course) {
        setTimeout(step3, 2000);
    }
    
    course_parent = tag_course.parentNode.parentNode.parentNode;
    is_full_siblings = course_parent.getElementsByTagName("td");
    is_full_node = is_full_siblings[is_full_siblings.length - 1];
    if (is_full_node.innerHTML == "人数未满") {
        tag_course.click();
        
        xlx = 1; // got it
        choose();
    }
    else { // go back and try again if it is full
        fh = main.document.getElementById("LessonTime1_Button1");
        if (!fh) {
            setTimeout(step3, 2000);
        }
        fh.click();
        
        // get a random interval time
        cnt++;
        total_cnt++;
        inter_time = Math.floor(Math.random() * 5000 + 1);
        if (cnt >= 40) {
            inter_time = inter_time + 30000;
            cnt = Math.floor(Math.random() * 5);
        }
        inter_time = 5000 + inter_time;
        console.log("Interval time 1: ", inter_time);
        setTimeout(step4, inter_time);
    }
}

function step4() { // find and click the taget course, go to select page
    courses = main.document.getElementsByName("myradiogroup");
    for (var i = 0; i < courses.length; ++i) {
        if (courses[i].value == "MU901") { // jiaoxiang yinyue jianshang
            courses[i].click();
            break;
        }
    }
    
    kcap = main.document.getElementById("lessonArrange");
    if (!kcap) setTimeout(step4, 1500);
    kcap.click();
    
    setTimeout(step5, 2000);
}

function step5() { // find taget teacher and judge whether is full
    teachers = main.document.getElementsByName("myradiogroup");
    tag_course = teachers[0];
    for (var i = 0; i < teachers.length; ++i) {
        if (teachers[i].value == 374512) {
            tag_course = teachers[i];
            break;
        }
    }
    
    if (!tag_course) {
        setTimeout(step5, 2000);
    }
    
    course_parent = tag_course.parentNode.parentNode.parentNode;
    is_full_siblings = course_parent.getElementsByTagName("td");
    is_full_node = is_full_siblings[is_full_siblings.length - 1];
    if (is_full_node.innerHTML == "人数未满") {
        tag_course.click();
        
        jxy = 1; // got it
        choose();
    }
    else { // go back and try again if it is full
        fh = main.document.getElementById("LessonTime1_Button1");
        if (!fh) {
            setTimeout(step5, 2000);
        }
        fh.click();
        
        // get a random interval time
        cnt++;
        total_cnt++;
        inter_time = Math.floor(Math.random() * 5000 + 1);
        if (cnt >= 40) {
            inter_time = inter_time + 30000;
            cnt = Math.floor(Math.random() * 5);
        }
        inter_time = 5000 + inter_time;
        console.log("Interval time 2: ", inter_time);
        setTimeout(step6, inter_time);
    }
}

function step6() { // find and click the taget course, go to select page
    courses = main.document.getElementsByName("myradiogroup");
    for (var i = 0; i < courses.length; ++i) {
        if (courses[i].value == "BI001") { // shengming kexue daolun
            courses[i].click();
            break;
        }
    }
    
    kcap = main.document.getElementById("lessonArrange");
    if (!kcap) setTimeout(step6, 1500);
    kcap.click();
    
    setTimeout(step7, 2000);
}

function step7() { // find taget teacher and judge whether is full
    teachers = main.document.getElementsByName("myradiogroup");
    tag_course = teachers[0];
    for (var i = 0; i < teachers.length; ++i) {
        if (teachers[i].value == 374529) {
            tag_course = teachers[i];
            break;
        }
    }
    
    if (!tag_course) {
        setTimeout(step7, 2000);
    }
    
    course_parent = tag_course.parentNode.parentNode.parentNode;
    is_full_siblings = course_parent.getElementsByTagName("td");
    is_full_node = is_full_siblings[is_full_siblings.length - 1];
    if (is_full_node.innerHTML == "人数未满") {
        tag_course.click();
        
        skd = 1; // got it
        choose();
    }
    else { // go back and try again if it is full
        fh = main.document.getElementById("LessonTime1_Button1");
        if (!fh) {
            setTimeout(step7, 2000);
        }
        fh.click();
        
        // get a random interval time
        cnt++;
        total_cnt++;
        inter_time = Math.floor(Math.random() * 5000 + 1);
        if (cnt >= 40) {
            inter_time = inter_time + 30000;
            cnt = Math.floor(Math.random() * 5);
        }
        inter_time = 5000 + inter_time;
        console.log("Interval time 3: ", inter_time);
        setTimeout(step8, inter_time);
    }
}

function step8() { // find and click the taget course, go to select page
    courses = main.document.getElementsByName("myradiogroup");
    for (var i = 0; i < courses.length; ++i) {
        if (courses[i].value == "CL915") { // ouzhou wenhua daolun
            courses[i].click();
            break;
        }
    }
    
    kcap = main.document.getElementById("lessonArrange");
    if (!kcap) setTimeout(step8, 1500);
    kcap.click();
    
    setTimeout(step9, 2000);
}

function step9() { // find taget teacher and judge whether is full
    teachers = main.document.getElementsByName("myradiogroup");
    tag_course = teachers[0];
    for (var i = 0; i < teachers.length; ++i) {
        if (teachers[i].value == 374559) {
            tag_course = teachers[i];
            break;
        }
    }
    
    if (!tag_course) {
        setTimeout(step9, 2000);
    }
    
    course_parent = tag_course.parentNode.parentNode.parentNode;
    is_full_siblings = course_parent.getElementsByTagName("td");
    is_full_node = is_full_siblings[is_full_siblings.length - 1];
    if (is_full_node.innerHTML == "人数未满") {
        tag_course.click();
        
        ozwh = 1; // got it
        setTimeout(choose, 700);
    }
    else { // go back and try again if it is full
        fh = main.document.getElementById("LessonTime1_Button1");
        if (!fh) {
            setTimeout(step9, 2000);
        }
        fh.click();
        
        // get a random interval time
        cnt++;
        total_cnt++;
        inter_time = Math.floor(Math.random() * 9000 + 1);
        if (cnt >= 40) {
            inter_time = inter_time + 20000;
            cnt = Math.floor(Math.random() * 5);
        }
        inter_time = 5000 + inter_time;
        console.log("Interval time 4: ", inter_time);
        setTimeout(step2, inter_time);
    }
}

function choose() { // choose the target teacher and click 
    xdcjs = main.document.getElementById("LessonTime1_btnChoose");
    if (!xdcjs) setTimeout(choose, 1000);
    xdcjs.click();
    
    // console.log("step4 to step5");
    setTimeout(submit, 1500);
}

function submit() { // submit
    xktj = main.document.getElementById("btnSubmit");
    xktj.click();
    
    console.log("Get a course!");
    if (xlx == 0) setTimeout(step2, 3000);
    else if (jxy == 0) setTimeout(step4, 3000);
    else if (skd == 0) setTimeout(step6, 3000);
    else if (ozwh == 0) setTimeout(step8, 3000);
    else alert("All done!")；
}

step2();



















