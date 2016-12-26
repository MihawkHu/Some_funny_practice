// script for courses in junior2
// using CS410(AI) as example
// 2016/12

var tag_course_index = 379764;
main = frames['main'];

var total_cnt = 0;
var cnt = 0;

function enter() {
    courses = main.document.getElementsByName("myradiogroup");
    for (var i = 0; i < courses.length; ++i) {
        if (courses[i].value == "CS410") {
            courses[i].click();
            break;
        }
    }
    
    kcap = main.document.getElementById("lessonArrange");
    kcap.click();
    
    setTimeout(judge, 2000);
}

function judge() {
    teachers = main.document.getElementsByName("myradiogroup");
    tag_course = teachers[0];
    for (var i = 0; i < teachers.length; ++i) {
        if (teachers[i].value == tag_course_index) {
            tag_course = teachers[i];
            break;
        }
    }
    
    if (!tag_course) {
        setTimeout(judge, 2000);
    }
    
    course_parent = tag_course.parentNode.parentNode.parentNode;
    is_full_siblings = course_parent.getElementsByTagName("td");
    is_full_node = is_full_siblings[is_full_siblings.length - 1];
    if (is_full_node.innerHTML == "人数未满") {
        tag_course.click();
        
        choose();
    }
    else {
        fh = main.document.getElementById("LessonTime1_Button1");
        if (!fh) {
            setTimeout(judge, 2000);
        }
        fh.click();
        
        cnt++;
        total_cnt++;
        inter_time = Math.floor(Math.random() * 10000 + 1);
        if (cnt >= 20) {
            inter_time = inter_time + 15000;
            cnt = Math.floor(Math.random() * 5);
        }
        inter_time = 3000 + inter_time;
        console.log("Interval time", total_cnt, ": ", inter_time, "ms");
        setTimeout(enter, inter_time);
    }
}

function choose() {
    xdcjs = main.document.getElementById("LessonTime1_btnChoose");
    xdcjs.click();
    
    setTimeout(submit, 3000);
}

function submit() {
    xktj = main.document.getElementById("btnSubmit");
    xktj.click();
    
    console.log("Mission Accomplishment!\n Total try times: ", total_cnt);
}

enter();



















