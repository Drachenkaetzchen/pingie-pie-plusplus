<?php

$shm_key = ftok("/tmp/foo", 'A');
$id = shmop_open($shm_key, "w", 0, 0);


$img = imagecreatefrompng("pingiepie.png");
$img2 = imagecreatefrompng("star.png");

$target = imagecreate(152,16);

$kk = 0;

while (1) {
	for ($y = 0;$y<imagesy($img)-16;$y++) {
		imagecopy($target, $img, 0, 0, 0, $y, 152, 16);
		if ($kk < 3) {
			imagecopy($target, $img2, 120, 0, 0, 0, 16, 16);
		}
		
		if ($kk == 5) {
			$kk = 0;
		}
		$kk++;
		copyToSHM($target, $id);
	}

	for ($y = imagesy($img)-16;$y>-1;$y--) {
		imagecopy($target, $img, 0, 0, 0, $y, 152, 16);
		if ($kk < 3) {
			imagecopy($target, $img2, 120, 0, 0, 0, 16, 16);
		}
		
		if ($kk == 5) {
			$kk = 0;
		}
		$kk++;
		
		copyToSHM($target, $id);
	}
}



function copyToSHM ($image, $shmid) {
	$data = "";
	for ($y=0;$y<16;$y++) {
		for ($x=0;$x<152;$x++) {
			if (imagecolorat($image, $x, $y) == 0) {
				$data .= chr(0);
			} else {
				$data .= chr(1);
			}
		}
	}
	shmop_write($shmid, $data, 0);	
}