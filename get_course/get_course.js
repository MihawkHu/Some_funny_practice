var tag_course_index = 374928; // course index
main = frames['main'];

var total_cnt = 0;
var cnt = 0;

function step1() { // find and click bixiu ke
    bxk = main.document.getElementById("btnBxk");
    bxk.click();
    
    console.log("step1 to step2");
    setTimeout(step2, 1500);
}

function step2() { // find and click the taget course, go to select page
    courses = main.document.getElementsByName("myradiogroup");
    for (var i = 0; i < courses.length; ++i) {
        if (courses[i].value == "CS214") {
            courses[i].click();
            break;
        }
    }
    
    kcap = main.document.getElementById("SpeltyRequiredCourse1_lessonArrange");
    kcap.click();
    
    console.log("step2 to step3");
    setTimeout(step3, 2000);
}

function step3() { // find taget teacher and judge whether is full
    teachers = main.document.getElementsByName("myradiogroup");
    tag_course = teachers[0];
    for (var i = 0; i < teachers.length; ++i) {
        if (teachers[i].value == tag_course_index) {
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
        
        console.log("step3 to step4");
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
        inter_time = Math.floor(Math.random() * 15000 + 1);
        if (cnt >= 20) {
            inter_time = inter_time + 30000;
            cnt = Math.floor(Math.random() * 5);
        }
        inter_time = 5000 + inter_time;
        console.log("Interval time: ", inter_time);
        setTimeout(step2, inter_time);
    }
}

function choose() { // choose the target teacher and click 
    xdcjs = main.document.getElementById("LessonTime1_btnChoose");
    xdcjs.click();
    
    console.log("step4 to step5");
    setTimeout(submit, 1500);
}

function submit() { // submit
    xktj = main.document.getElementById("SpeltyRequiredCourse1_Button1");
    xktj.click();
    
    console.log("Mission Accomplishment!\n Total try times: ", total_cnt);
}

step1();



















