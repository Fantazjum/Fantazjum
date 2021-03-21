import * as THREE from './build/three.module.js';
import {OrbitControls} from './controls/OrbitControls.js';
	
function main() {
  const canvas = document.querySelector('#glCanvas');
  const renderer = new THREE.WebGLRenderer({ canvas });

  const fov = 75;
  const aspect = 2; 
  const near = 0.1;
  const far = 1000;
  const camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
  camera.position.y = 5
  camera.position.z = 15;
  
  const controls = new OrbitControls(camera,canvas);
  controls.target.set(0,0,0);
  controls.enablePan = false; 
  controls.update();

  const scene = new THREE.Scene();
  
  function lighting(...pos)
  {
	const color = 0xFFFFFF;
    const intensity = 1;
    const light = new THREE.DirectionalLight(color, intensity);
    light.position.set(...pos);
    scene.add(light);
	}
	
	lighting(-1,2,5);
	lighting(1,2,-3);

  
  const snowflake = new THREE.Geometry();
  snowflake.vertices.push(
	new THREE.Vector3(0,0,0.1), //0
	new THREE.Vector3(0,0.2,0), //1
	new THREE.Vector3(0.1,0.1,0), //2
	new THREE.Vector3(0.3,0.1,0), //3
	new THREE.Vector3(0.2,0,0), //4
	new THREE.Vector3(0.3,-0.1,0), //5
	new THREE.Vector3(0.1,-0.1,0), //6
	new THREE.Vector3(0,-0.2,0), //7
	new THREE.Vector3(-0.1,-0.1,0), //8
	new THREE.Vector3(-0.3,-0.1,0), //9
	new THREE.Vector3(-0.2,0,0), //10
	new THREE.Vector3(-0.3,0.1,0), //11
	new THREE.Vector3(-0.1,0.1,0), //12
	new THREE.Vector3(0,0,-0.1), //13
  );
  snowflake.faces.push(
     new THREE.Face3(0,2,1),
     new THREE.Face3(0,3,2),
     new THREE.Face3(0,4,3),
     new THREE.Face3(0,5,4),
     new THREE.Face3(0,6,5),
     new THREE.Face3(0,7,6),
     new THREE.Face3(0,8,7),
     new THREE.Face3(0,9,8),
     new THREE.Face3(0,10,9),
     new THREE.Face3(0,11,10),
     new THREE.Face3(0,12,11),
     new THREE.Face3(0,1,12),

     new THREE.Face3(13,1,2),
     new THREE.Face3(13,2,3),
     new THREE.Face3(13,3,4),
     new THREE.Face3(13,4,5),
     new THREE.Face3(13,5,6),
     new THREE.Face3(13,6,7),
     new THREE.Face3(13,7,8),
     new THREE.Face3(13,8,9),
     new THREE.Face3(13,9,10),
     new THREE.Face3(13,10,11),
     new THREE.Face3(13,11,12),
     new THREE.Face3(13,12,1),	 
  );
  
  function resizeRendererToDisplaySize(renderer) {
      const canvas = renderer.domElement;
      const width = canvas.clientWidth;
      const height = canvas.clientHeight;
      const needResize = canvas.width !== width || canvas.height !== height;
      if (needResize) {
        renderer.setSize(width, height, false);
      }
      return needResize;
  }
  
  snowflake.computeVertexNormals();
  
  const loader = new THREE.TextureLoader();
  const presentMat = new THREE.MeshPhongMaterial({ 
  map: loader.load('textures/Box-side.png'),
  });
  const presentGeom = new THREE.BoxGeometry(5,5,5);
  const present = new THREE.Mesh(presentGeom,presentMat);
  scene.add(present);
  present.rotation.y = 0.5;
  present.position.x = -5;
  
  
  function makeSnow(x,y,z) {
    const material = new THREE.MeshPhongMaterial(0xFFFFFF);

    const cold = new THREE.Mesh(snowflake, material);
    scene.add(cold);

    cold.position.x = x;
	cold.rotation.y = Math.random();
	cold.rotation.z = Math.random();
    cold.position.y = Math.random()*9-6+y;
    cold.position.z = Math.random()*6-9+z;
    return cold;
  }
  
  const snowmanBottom = new THREE.MeshPhongMaterial({color : 0xFFFFFF});
  const bottomBall = new THREE.SphereGeometry( 3, 20, 20);
  const bottomer = new THREE.Mesh(bottomBall, snowmanBottom);
  scene.add(bottomer);
  bottomer.position.x = 3;
  
  const snowmanMiddle = new THREE.MeshPhongMaterial({
	  map : loader.load('textures/Torso.png'),
  });
  const middleBall = new THREE.SphereGeometry(2, 15, 15);
  const middle = new THREE.Mesh(middleBall, snowmanMiddle);
  scene.add(middle);
  middle.position.x = 3;
  middle.position.y = 4.2;
  middle.rotation.y = -1.5;
  
  const snowmanTop = new THREE.MeshPhongMaterial({
	  map : loader.load('textures/Smiley.png'),
  });
  const topBall = new THREE.SphereGeometry(1, 10, 10);
  const topper = new THREE.Mesh(topBall, snowmanTop);
  scene.add(topper);
  topper.position.x = 3;
  topper.position.y = 7;
  topper.rotation.y = -1.5;
  
  const hatRing = new THREE.MeshBasicMaterial({color: 0x000000, side: THREE.DoubleSide});
  const ring = new THREE.RingGeometry(0.5, 1.5, 15);
  const topRing = new THREE.Mesh(ring, hatRing);
  scene.add(topRing);
  topRing.position.x = 3;
  topRing.position.y = 7.8;
  topRing.rotation.x = 33;
  
  const topCylinder = [
	new THREE.MeshPhongMaterial({map : loader.load('textures/Hat.png')}),
	new THREE.MeshPhongMaterial({color: 0x000000}),
	new THREE.MeshPhongMaterial({color: 0x000000}),
	];
  const cylinderShape = new THREE.CylinderGeometry(1.2,1,2,12);
  const tophat = new THREE.Mesh(cylinderShape, topCylinder);
  scene.add(tophat);
  tophat.position.x = 3;
  tophat.position.y = 8.7;
  
  const back = loader.load('textures/Back.jpg',
		() => {
			const rt = new THREE.WebGLCubeRenderTarget(back.image.height);
			rt.fromEquirectangularTexture(renderer, back);
			scene.background = rt;
		});

  const snow = [
	makeSnow(-19,-6,11), makeSnow(-18,-6,0), makeSnow(-17,-6,11),
	makeSnow(-16,-6,0), makeSnow(-15,-6,11), makeSnow(-14,-6,0),
	makeSnow(-13,-6,11), makeSnow(-12,-6,0), makeSnow(-11,-6,11),
	makeSnow(-10,-3,0), makeSnow(-9,-3,11), makeSnow(-8,-3,0),
    makeSnow(-7,-3,11), makeSnow(-6,-3,0), makeSnow(-5,-3,11),
    makeSnow(-4,-3,0), makeSnow(-3,-3,11), makeSnow(-2,-3,0),
	makeSnow(-1,0,11), makeSnow(0,0,0), makeSnow(1,0,11),
    makeSnow(2,3,0), makeSnow(3,3,11), makeSnow(4,3,0),
	makeSnow(5,3,11), makeSnow(6,3,0), makeSnow(7,3,11),
	makeSnow(8,3,0), makeSnow(9,3,11), makeSnow(10,3,0),
	makeSnow(11,6,11), makeSnow(12,6,0), makeSnow(13,6,11),
	makeSnow(14,6,0), makeSnow(15,6,11), makeSnow(16,6,0),
	makeSnow(17,6,11), makeSnow(18,6,0), makeSnow(19,6,11),
  ];
  
  function render(time) {
	if (resizeRendererToDisplaySize(renderer)) {
      const canvas = renderer.domElement;
      camera.aspect = canvas.clientWidth / canvas.clientHeight;
      camera.updateProjectionMatrix();
    }
	snow.forEach(cold => {
	cold.rotation.y += 0.05;
	cold.rotation.z += 0.01;
	cold.position.x += 0.1;
	if(cold.position.x > 20) 
		cold.position.x = -20;
	cold.position.y -= 0.1;
	if(cold.position.y < -10) 
		cold.position.y = 10;
	});
	renderer.render(scene, camera);
    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
  
}

main();
