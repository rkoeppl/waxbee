/*
 * Wacom-related stuff for the USB-side interface
 *
 */

#include "usb_util.h"
#include "extdata.h"
#include "console.h"
#include "wacom_usb.h"
#include "pen_event.h"
#include "pen_data_transform.h"
#include "strings.h"
#include "debugproc.h"

#define WACOM_PEN_ENDPOINT	1

namespace WacomUsb
{
	//-----------------------------------------------------------------
	// USB Protocol IV (4)
	//-----------------------------------------------------------------

	struct protocol4_struct
	{
		union
		{
			uint8_t buffer[8];
			struct
			{
				uint8_t hid_identifier; // 0x02
				struct
				{
					unsigned touch:1;   // bit 0 - pen touches the tablet
					unsigned button0:1; // bit 1 - first button on the pen
					unsigned button1:1; // bit 2 - second button on the pen
					unsigned reserved:1;// bit 3 - appears to be always 0
					unsigned inrange:1; // bit 4 - like 'proximity' but for a greater distance.
							    // 		mouse is not moving in Windows when in that range
							    // 		but x/y coordinates are being sent.
					unsigned eraser:1;  // bit 5 - eraser tool
					unsigned is_mouse:1;// bit 6 - 0=pen, 1=mouse

					unsigned proximity:1;//bit 7 - pen in close proximity, moves the mouse
				};
				uint16_t x;
				uint16_t y;
				unsigned pressure:9;
				unsigned zero:7;	// 0
			};
		};
	} protocol4_packet;

	void send_protocol4_packet(Pen::PenEvent& penEvent)
	{
		bool shouldfill = false;
		// copy event for direct access (TODO: does that make a difference?)

		protocol4_packet.hid_identifier = 0x02;

		protocol4_packet.reserved = 0;
		protocol4_packet.is_mouse = penEvent.is_mouse;
		protocol4_packet.proximity = penEvent.proximity;
		protocol4_packet.inrange = penEvent.proximity;

		if(protocol4_packet.inrange)
		{
			PenDataTransform::XformedData xformed;

			PenDataTransform::transform_pen_data(penEvent, xformed,
								false); // no tilt

			protocol4_packet.x = xformed.x;
			protocol4_packet.y = xformed.y;
			protocol4_packet.pressure = xformed.pressure;
			protocol4_packet.touch = penEvent.touch;
			protocol4_packet.eraser = penEvent.eraser;
			protocol4_packet.button0 = penEvent.button0;
			protocol4_packet.button1 = penEvent.button1;

			shouldfill = true;
		}
		else
		{
			protocol4_packet.x = 0;
			protocol4_packet.y = 0;
			protocol4_packet.pressure = 0;
			protocol4_packet.touch = 0;
			protocol4_packet.eraser = 0;
			protocol4_packet.button0 = 0;
			protocol4_packet.button1 = 0;

			DebugProc::proxOutTrigger();
		}

/*		if(console::console_enabled)
		{
			console::print("[USB Packet - prox:");
			console::printbit(protocol4_packet.proximity);
			console::print(" mou:");
			console::printbit(protocol4_packet.is_mouse);
			console::print(" range:");
			console::printbit(protocol4_packet.inrange);
			console::print(" touc:");
			console::printbit(protocol4_packet.touch);
			console::print(" era:");
			console::printbit(protocol4_packet.eraser);
			console::print(" b0:");
			console::printbit(protocol4_packet.button0);
			console::print(" b1:");
			console::printbit(protocol4_packet.button1);
			console::print(" res:");
			console::printbit(protocol4_packet.reserved);
			console::print(" x=");
			console::printNumber(protocol4_packet.x);
			console::print(", y=");
			console::printNumber(protocol4_packet.y);
			console::print(", pressure=");
			console::printNumber(protocol4_packet.pressure);
			console::println("]");
		}
*/
		if(extdata_getValue8(EXTDATA_USB_PORT) == EXTDATA_USB_PORT_DIGITIZER)
			UsbUtil::send_packet_fill_idle_time(protocol4_packet.buffer, 8, WACOM_PEN_ENDPOINT, 50);
	}

	//-----------------------------------------------------------------
	// USB Bamboo Pen Packet
	//-----------------------------------------------------------------

	struct bamboo_pen_struct
	{
		union
		{
			uint8_t buffer[9];
			struct
			{
				uint8_t hid_identifier; // 0x02
				struct
				{
					unsigned touch:1;   // bit 0 - pen touches the tablet
					unsigned button0:1; // bit 1 - first button on the pen
					unsigned button1:1; // bit 2 - second button on the pen
					unsigned eraser:1;  // bit 3 - eraser tool
					unsigned inrange:1; // bit 4 - (?)like 'proximity' but for a greater distance.
							    // 		mouse is not moving in Windows when in that range
							    // 		but x/y coordinates are being sent.
					unsigned proximity:1;// bit 5 - proximity(?) pen in close proximity, moves the mouse
					unsigned notsure:1; // bit 6 - ??
					unsigned outofprox:1;// bit 7 - appears to be the only thing set with everything else is zero?
				};
				uint16_t x;
				uint16_t y;
				uint16_t pressure;
				uint8_t distance;
			};
		};
	} bamboo_pen_packet;

	void send_bamboo_pen_packet(Pen::PenEvent& penEvent)
	{
		bool shouldfill = false;
		bamboo_pen_packet.hid_identifier = 0x02;

		bamboo_pen_packet.notsure = 0;

//		bamboo_pen_packet.is_mouse = penEvent.is_mouse; // no mouse with bamboo(?)
		bamboo_pen_packet.inrange = penEvent.proximity;

		if(bamboo_pen_packet.inrange)
		{
			PenDataTransform::XformedData xformed;

			PenDataTransform::transform_pen_data(penEvent, xformed,
								false); // no tilt

			bamboo_pen_packet.x = xformed.x;
			bamboo_pen_packet.y = xformed.y;
			bamboo_pen_packet.pressure = xformed.pressure;
			bamboo_pen_packet.proximity = penEvent.proximity;
			bamboo_pen_packet.touch = penEvent.touch;
			bamboo_pen_packet.eraser = penEvent.eraser;
			bamboo_pen_packet.button0 = penEvent.button0;
			bamboo_pen_packet.button1 = penEvent.button1;
			bamboo_pen_packet.distance = 0x1A; // not sure if that value is used(?)
			bamboo_pen_packet.outofprox = 0;

			shouldfill = true;
		}
		else
		{
			bamboo_pen_packet.x = 0;
			bamboo_pen_packet.y = 0;
			bamboo_pen_packet.proximity = 0;
			bamboo_pen_packet.pressure = 0;
			bamboo_pen_packet.distance = 0;
			bamboo_pen_packet.touch = 0;
			bamboo_pen_packet.eraser = 0;
			bamboo_pen_packet.button0 = 0;
			bamboo_pen_packet.button1 = 0;
			bamboo_pen_packet.outofprox = 1;

			DebugProc::proxOutTrigger();
		}

		if(console::console_enabled)
		{
			console::print("[USB Packet - prox:");
			console::printbit(bamboo_pen_packet.proximity);
			console::print(" outofprox:");
			console::printbit(bamboo_pen_packet.outofprox);
			console::print(" range:");
			console::printbit(bamboo_pen_packet.inrange);
			console::print(" touc:");
			console::printbit(bamboo_pen_packet.touch);
			console::print(" era:");
			console::printbit(bamboo_pen_packet.eraser);
			console::print(" b0:");
			console::printbit(bamboo_pen_packet.button0);
			console::print(" b1:");
			console::printbit(bamboo_pen_packet.button1);
			console::print(" x=");
			console::printNumber(bamboo_pen_packet.x);
			console::print(", y=");
			console::printNumber(bamboo_pen_packet.y);
			console::print(", pressure=");
			console::printNumber(bamboo_pen_packet.pressure);
			console::print(", distance=");
			console::printNumber(bamboo_pen_packet.distance);
			console::println("]");
		}

		if(extdata_getValue8(EXTDATA_USB_PORT) == EXTDATA_USB_PORT_DIGITIZER)
			UsbUtil::send_packet_fill_idle_time(bamboo_pen_packet.buffer, 9, WACOM_PEN_ENDPOINT, 50);
	}

	struct bamboo_touch_struct
	{
		union
		{
			uint8_t buffer[20];
			struct
			{
				uint8_t hid_identifier; // 0x2
			};
		};
	} bamboo_touch_packet;

	void send_bamboo_touch_packet(Touch::TouchEvent& touchEvent)
	{
		if(!touchEvent.touch)
		{
			for(int i=0;i<20;i++)
				bamboo_touch_packet.buffer[i] = 0;
			bamboo_touch_packet.hid_identifier = 0x2;
		}
		else
		{
			return;
		}

		if(extdata_getValue8(EXTDATA_USB_PORT) == EXTDATA_USB_PORT_DIGITIZER)
			UsbUtil::send_packet(bamboo_touch_packet.buffer, 20, WACOM_PEN_ENDPOINT, 50);
	}

	//-----------------------------------------------------------------
	// USB Protocol V (5)
	//
	// Data gleaned for the Intuos2
	// Button 0 = side switch near the tip (not working with eraser)
	// Button 1 = side switch away from the tip (not working with eraser)
	//
	//	02 C2 85 20 16 00 2C D0 00 00   In Range: NORMAL PEN
	//	02 C2 85 A0 16 00 2C D0 00 00   In Range: ERASER (PEN)
	//
	//	02 A0 91 06 66 9F 00 20 40 B8   Pen Data (see below)
	//	...
	//	...
	//
	//	02 80 00 00 00 00 00 00 00 00   Out of range
	//
	//
	// Pen Data:	02 A0 91 06 66 9F 00 20 40 B8
	//
	// [0]	02  HID report identifier (02)
	// [1]	A0  1 1 p 0  0 b c i   p = prox, b = button1, c = button0, i = index
	// [2]	91  x x x x  x x x x   x pos
	// [3]	06  x x x x  x x x x
	// [4]	66  y y y y  y y y y   y pos
	// [5]	9F  y y y y  y y y y
	// [6]	00  p p p p  p p p p   pressure (1024)
	// [7]	20  p p t t  t t t t   t = tiltx - 0 = -60 degrees, 40 = 0, 7F = +60 degrees
	// [8]	40  t u u u  u u u u   u = tilty - 0 = -60 degrees, 40 = 0, 7F = +60 degrees
	// [9]	B8  d d d d  d 0 0 0   d = sensor z distance 0x26..0x0D
	//
	// touch = pressure > 10
	//
	//-----------------------------------------------------------------

	struct protocol5_struct
	{
		union
		{
			uint8_t buffer[10];
			struct
			{
				uint8_t hid_identifier; // 0x02
				struct
				{
					unsigned index:1;   // bit 0 - tool index -- set to 0
					unsigned button0:1; // bit 1 - first button on the pen
					unsigned button1:1; // bit 2 - second button on the pen
					unsigned bit3:1;    // bit 3 - set to 0 (for pen)
					unsigned bit4:1;    // bit 4 - set to 0 (for pen)
					unsigned proximity:1;//bit 5 - when 0, pen is far from the surface but
							    // mouse is moving in Windows.
					unsigned bit6:1;    // bit 6 - set to 1 (for pen)
					unsigned bit7:1;    // bit 7 - set to 1 (for pen)
				};
				uint8_t x_high;
				uint8_t x_low;
				uint8_t y_high;
				uint8_t y_low;
				uint8_t byte6;			// pressure high
				uint8_t byte7;			// pressure + tiltx
				uint8_t byte8;			// tiltx + tilty
				unsigned zero:3;		// 0 (bits 0..2)
				unsigned distance:5;		// distance from the sensor -
								// seen numbers from 0x26 (far) to 0x0D (close) (bits 3..7)
			};
		};
	} protocol5_packet;

	static bool currentlyInRange = false;

	static void do_send_protocol5_packet(Pen::PenEvent& penEvent)
	{
		bool shouldfill = false;

		uint8_t* pbuf = protocol5_packet.buffer;

		*pbuf++ = 0x02; // [0] - HID report identifier

		if(penEvent.proximity)
		{
			if(currentlyInRange)
			{
				// normal pen packet
				protocol5_packet.index = 0;
				protocol5_packet.button0 = penEvent.button0;
				protocol5_packet.button1 = penEvent.button1;
				protocol5_packet.bit3 = 0;
				protocol5_packet.bit4 = 0;
				protocol5_packet.proximity = 1; // check with distance maybe?
				protocol5_packet.bit6 = 1;
				protocol5_packet.bit7 = 1;

				PenDataTransform::XformedData xformed;

				PenDataTransform::transform_pen_data(penEvent, xformed,
									true); // with tilt

				protocol5_packet.x_high = ((xformed.x >> 8) & 0xff);
				protocol5_packet.x_low = (xformed.x & 0xff);

				protocol5_packet.y_high = ((xformed.y >> 8) & 0xff);
				protocol5_packet.y_low = (xformed.y & 0xff);

				uint8_t tiltx = xformed.tilt_x + 0x40;
				uint8_t tilty = xformed.tilt_y + 0x40;

				protocol5_packet.byte6 = (xformed.pressure >> 2) & 0xFF;
				protocol5_packet.byte7 = ((xformed.pressure & 0x3) << 6) | ((tiltx >> 1) & 0x3F);
				protocol5_packet.byte8 = ((tiltx << 7) & 0x80) | ( tilty & 0x7F);

				protocol5_packet.distance = 0x0D;  // we must be getting this value from ADB or Serial tablets, no??

/*				if(console::console_enabled)
				{
					console::print("[USB Packet - prox:");
					console::printbit(protocol5_packet.proximity);
					console::print(" b0:");
					console::printbit(protocol5_packet.button0);
					console::print(" b1:");
					console::printbit(protocol5_packet.button1);
					console::print(" x:");
					console::printNumber((((uint16_t)protocol5_packet.x_high) << 8) | protocol5_packet.x_low);
					console::print(" y:");
					console::printNumber((((uint16_t)protocol5_packet.y_high) << 8) | protocol5_packet.y_low);
					console::print(" pressure:");
					console::printNumber((((uint16_t)protocol5_packet.byte6) << 2) |
							(((uint16_t)protocol5_packet.byte7) >> 6));
					console::print(" byte6,7,8:");
					console::printHex(protocol5_packet.byte6,2);
					console::printHex(protocol5_packet.byte7,2);
					console::printHex(protocol5_packet.byte8,2);
					console::println("]");
				}
*/
			}
			else
			{
				currentlyInRange = true;

				// pen getting into range
				//
				// 	02 C2 85 20 16 00 2C D0 00 00   In Range: NORMAL PEN
				//	02 C2 85 A0 16 00 2C D0 00 00   In Range: ERASER (PEN)

				// To simplify the logic, "replace" the first packet with a proximity packet
				// (will thus loose the first packet when first approaching the pen, but that
				// should not be a problem I think)

				*pbuf++ = 0xC2; // [1]
				*pbuf++ = 0x85; // [2]

				if(penEvent.eraser)
					*pbuf++ = 0xA0; // [3]
				else
					*pbuf++ = 0x20; // [3]

				*pbuf++ = 0x16; // [4]
				*pbuf++ = 0x00; // [5]
				*pbuf++ = 0x2C; // [6]
				*pbuf++ = 0xD0; // [7]
				*pbuf++ = 0x00; // [8]
				*pbuf   = 0x00; // [9]

				if(console::console_enabled)
				{
					console::printP(STR_USB_PACKET_IN_RANGE_PACKET_ERASER);
					console::printbit(penEvent.eraser);
					console::println("]");
				}
			}

			shouldfill = true;
		}
		else if(currentlyInRange)
		{
			currentlyInRange = false;

			// out of range  -- 02 80 00 00 00 00 00 00 00 00

			*pbuf++ = 0x80; // [1]
			*pbuf++ = 0x00; // [2]
			*pbuf++ = 0x00; // [3]
			*pbuf++ = 0x00; // [4]
			*pbuf++ = 0x00; // [5]
			*pbuf++ = 0x00; // [6]
			*pbuf++ = 0x00; // [7]
			*pbuf++ = 0x00; // [8]
			*pbuf   = 0x00; // [9]

			if(console::console_enabled)
			{
				console::printlnP(STR_USB_PACKET_OUT_OF_RANGE_PACKET_ALL_ZEROS);
			}

			DebugProc::proxOutTrigger();
		}
		else
		{
			// invalid state -- do not send anything
			return;
		}

		if(extdata_getValue8(EXTDATA_USB_PORT) == EXTDATA_USB_PORT_DIGITIZER)
			UsbUtil::send_packet_fill_idle_time(protocol5_packet.buffer, 10, WACOM_PEN_ENDPOINT, 50);
	}

	bool in_special_operation = false;
	bool special_op_inrange_state = false;

	void send_protocol5_packet(Pen::PenEvent& penEvent)
	{
		if(in_special_operation)
			return; // already in a special operation!

		do_send_protocol5_packet(penEvent);
	}

	/**
	 * get the device out of proximity momentarily
	 */
	void begin_special_operation_protocol5()
	{
		if(in_special_operation)
			return;	// already in a special operation!

		in_special_operation = true;
		special_op_inrange_state = currentlyInRange;

		if(currentlyInRange)
		{
			Pen::PenEvent penEvent;

			penEvent.proximity = false;

			do_send_protocol5_packet(penEvent);
		}
	}

	/**
	 * return to the state before the device was out of prox momentarily
	 */
	void end_special_operation_protocol5()
	{
		if(!in_special_operation)
			return;	// was not in a special operation

		in_special_operation = false;

		if(special_op_inrange_state)
		{
			// do nothing. Let the next pen movement re-initialize the status
		}

		special_op_inrange_state = false;
	}
}

