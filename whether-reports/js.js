const search_wheather_tab=document.querySelector("#search_wheather_tab");
const my_wheather_tab=document.querySelector("#my_wheather_tab");
const grant_access=document.querySelector(".grant_access");
const search_bar=document.querySelector(".search_bar");
const loding_screen=document.querySelector(".loding_screen");
const whetherdata=document.querySelector(".whetherdata");

const API_key='d1845658f92b31c64bd94f06f7188c9c';
let current_tab=my_wheather_tab;
current_tab.classList.add("current_tab");


function switch_screen(clicked_tab){

   if(current_tab!=clicked_tab){
        current_tab.classList.remove("current_tab");
        current_tab=clicked_tab;
        current_tab.classList.add("current_tab");
        if(!search_bar.classList.contains("active")){
            whetherdata.classList.remove("avtive");
            grant_access.classList.remove("active");
            search_bar.classList.add("active");
        }
        else{
            search_bar.classList.remove("active");
            whetherdata.classList.remove("avtive");
            check_access();
        }
   } 
}

function check_access(){
    const local_cordinates= sessionStorage.getItem('cordinate_data');
    if(!local_cordinates){
        grant_access.classList.add("active");
    }
    else{
        const cordinate=JSON.parse(local_cordinates);
        meramosam(cordinate);
    }

}

const access_button=document.querySelector("#grant_access_button");
access_button.addEventListener("click",my_location);

function my_location(){
    if(navigator.geolocation){
        navigator.geolocation.getCurrentPosition(showposition);
    }
    else{
        console.log("failed to get target location");        
    }

}

function showposition(position){
    const cordinatedata = { lat: position.coords.latitude,lon: position.coords.longitude,}
    sessionStorage.setItem("cordinate_data",JSON.stringify(cordinatedata));
    meramosam(cordinatedata);

}

async function meramosam(cordinate){
    const {lat ,lon}= cordinate;
    grant_access.classList.remove("active");
    loding_screen.classList.add("active");
    try{
    const api_la = await fetch(`https://api.openweathermap.org/data/2.5/weather?lat=${lat}&lon=${lon}&appid=${API_key}`);
    const data = await api_la.json();
    loding_screen.classList.remove("active");
    whetherdata.classList.add("active");
    publish_data(data);

    }
    catch(e){
        console.log("unable to fetch data",e);
    }
}

function publish_data(data){
    const location_name =document.querySelector("#location_name");
    const location_flag =document.querySelector("#location_flag");
    const sky_situation =document.querySelector(".sky_situation");
    const skyemoticon =document.querySelector(".skyemoticon");
    const temperature =document.querySelector(".temperature");
    const windspeed_data =document.querySelector("#windspeed_data");
    const humidity_data =document.querySelector("#humidity_data");
    const clouds_data =document.querySelector("#clouds_data");
    location_name.innerText=data?.name;
    location_flag.src = `https://flagcdn.com/144x108/${data?.sys?.country.toLowerCase()}.png`;
    sky_situation.innerText = data?.weather?.[0]?.description;
    skyemoticon.src = `http://openweathermap.org/img/w/${data?.weather?.[0]?.icon}.png`;
    temperature.innerText = `${data?.main?.temp} °C`;
    windspeed_data.innerText = `${data?.wind?.speed} m/s`;
    humidity_data.innerText = `${data?.main?.humidity}%`;
    clouds_data.innerText = `${data?.clouds?.all}%`;
}

const search_input=document.querySelector("#search_bar_input");
search_bar.addEventListener("submit", (e)=> {
    e.preventDefault();
    const city= search_input.value;
    console.log("data got in");
    console.log(city);
    if (city===""){
        return;
    }
    else{
        mosam(city);
    }

})

async function mosam(city){
    loding_screen.classList.add("active");
    whetherdata.classList.remove("active");
    grant_access.classList.remove("active");
    try{
    const api_la = await fetch(`https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${API_key}`);
    const data = await api_la.json();
    loding_screen.classList.remove("active");
    whetherdata.classList.add("active");
    console.log("data",data);
    publish_data(data);
    }
    catch(e){
        console.log("unable to fetch data",e);
    }
} 


 



search_wheather_tab.addEventListener("click",()=>{
    switch_screen(search_wheather_tab);
})

my_wheather_tab.addEventListener("click",()=>{
    switch_screen(my_wheather_tab);
})