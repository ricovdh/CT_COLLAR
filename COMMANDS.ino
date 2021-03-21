/** commands **/

String commands(String message) {

  String return_msg;
  if (message.endsWith("vib1")) {
    vib1();
    return_msg = "vib1";
  }
  if (message.endsWith("vib2")) {
    vib2();
    return_msg = "vib2";
  }
  if (message.endsWith("vib3")) {
    vib3();
    return_msg = "vib3";
  }
  if (message.endsWith("vib4")) {
    vib4();
    return_msg = "vib4";
  }

  if (message.endsWith("buz1")) {
    buz1();
    return_msg = "buz1";
  }
  if (message.endsWith("buz2")) {
    buz2();
    return_msg = "buz2";
  }
  if (message.endsWith("buz3")) {
    buz3();
    return_msg = "buz3";
  }

  return return_msg;
}