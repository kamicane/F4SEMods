package
{
	import Shared.AS3.*;
	import Shared.IMenu;
	import flash.display.*;
	import flash.events.*;
	import flash.geom.*;
	import flash.net.*;
	import flash.system.*;
	import flash.text.*;
	import flash.ui.*;

	import scaleform.gfx.*;

	public class TextInputMenu extends IMenu
	{

		public var Background_mc:MovieClip;
		public var ButtonHintBar_mc:BSButtonHintBar;

		public var CancelButton:BSButtonHintData;
		public var ConfirmButton:BSButtonHintData;

		public var BGSCodeObj:Object;

		public var TextInput_tf:TextField;
		public var Title_tf:TextField;

		public var Mask_mc:MovieClip;
		public var Test_mc:MovieClip;

		public function TextInputMenu()
		{
			trace("[TextInput] Initialize: super duper edition");

			// these need to be before super() for some reason
			this.CancelButton = new BSButtonHintData("$CLOSE", "Tab", "PSN_B", "Xenon_B", 1, this.onCancelPressed);
			this.ConfirmButton = new BSButtonHintData("$CONFIRM", "Enter", "PSN_A", "Xenon_A", 1, this.onConfirmPressed);

			// this.CancelButton.ButtonDisabled = false;
			// this.ConfirmButton.ButtonDisabled = false;

			// this.CancelButton.ButtonVisible = true;
			// this.ConfirmButton.ButtonVisible = true;

			super();
			this.BGSCodeObj = new Object();
			this.PopulateButtonBar();

			// this.UpdateMask();

			trace("[TextInput] Init done.");
		}

		public function PopulateButtonBar():void
		{
			trace("[TextInput] PopulateButtonBar: begin");

			var buttonHints:Vector.<BSButtonHintData> = new Vector.<BSButtonHintData>();
			trace("[TextInput] PopulateButtonBar: buttonHints built");

			buttonHints.push(this.CancelButton);
			buttonHints.push(this.ConfirmButton);

			trace("[TextInput] PopulateButtonBar: buttonHints pushed");
			this.ButtonHintBar_mc.SetButtonHintData(buttonHints);
			// this.ButtonHintBar_mc.bRedirectToButtonBarMenu = false;

			trace("[TextInput] PopulateButtonBar: done");
		}

		public function UpdateMask():void
		{
			trace("UpdateMask start");
			var gr:Graphics = Mask_mc.graphics;

			gr.clear();

			var padStage:Number = 10; // padding from stage right edge
			var padText:Number = 10; // padding to the right of the title
			var metrics:TextLineMetrics = Title_tf.getLineMetrics(0);

			var titleRight:Number = Title_tf.x + metrics.x + metrics.width;
			var titleBottom:Number = Title_tf.y + metrics.height;

			var rightBoxLeft:Number = titleRight + padText;
			var rightBoxRight:Number = 640 - padStage;
			var rightBoxWidth:Number = rightBoxRight - rightBoxLeft;
			var rightBoxHeight:Number = 100;
			var rightBoxTop:Number = Title_tf.y;
			var rightBoxBottom:Number = rightBoxTop + rightBoxHeight;

			gr.beginFill(0xFF0000);
			gr.drawRect(rightBoxLeft, rightBoxTop, rightBoxWidth, rightBoxHeight);
			gr.endFill();

			// Left rectangle: from -630 to left of the title minus padding
			var leftBoxLeft:Number = -640 + padStage; // -630
			var leftBoxRight:Number = Title_tf.x - padText;
			var leftBoxWidth:Number = leftBoxRight - leftBoxLeft;
			var leftBoxHeight:Number = 100;
			var leftBoxTop:Number = Title_tf.y;
			var leftBoxBottom:Number = leftBoxTop + leftBoxHeight;

			gr.beginFill(0x00FF00);
			gr.drawRect(leftBoxLeft, leftBoxTop, leftBoxWidth, leftBoxHeight);
			gr.endFill();

			// Bottom rectangle: from -630 at text bottom to stage bottom (360)
			var bottomBoxLeft:Number = -640 + padStage;
			var bottomBoxRight:Number = rightBoxRight;
			var bottomBoxTop:Number = titleBottom;
			var bottomBoxBottom:Number = 360 - padStage;
			var bottomBoxWidth:Number = bottomBoxRight - bottomBoxLeft;
			var bottomBoxHeight:Number = bottomBoxBottom - bottomBoxTop;

			gr.beginFill(0x0000FF);
			gr.drawRect(bottomBoxLeft, bottomBoxTop, bottomBoxWidth, bottomBoxHeight);
			gr.endFill();

			trace("UpdateMask end");
		}

		public function SetText(text:String):void
		{
			this.TextInput_tf.text = text;
		}

		public function SetTitle(text:String):void
		{
			this.Title_tf.text = text;
		}

		public function InitializeTextInput():void
		{
			this.TextInput_tf.type = TextFieldType.INPUT;
			this.TextInput_tf.selectable = true;
			this.TextInput_tf.maxChars = 28;
			stage.focus = this.TextInput_tf;
			this.TextInput_tf.setSelection(0, this.TextInput_tf.text.length);

			addEventListener(KeyboardEvent.KEY_DOWN, this.onKeyDown);
			addEventListener(KeyboardEvent.KEY_UP, this.onKeyUp);
		}

		public function onCodeObjCreate():*
		{
			trace("[TextInput] onCodeObjCreate");
			this.BGSCodeObj.Initialize();

			this.InitializeTextInput();
			this.UpdateMask();
		}

		public function onCodeObjDestruction():*
		{
			trace("[TextInput] onCodeObjDestruction");
			this.BGSCodeObj = null;
		}

		public function onCancelPressed():void
		{
			trace("[TextInput] onCancelPressed");
			this.BGSCodeObj.Cancel();
		}

		public function onConfirmPressed():void
		{
			trace("[TextInput] onConfirmPressed. Text = '" + this.TextInput_tf.text + "'");
			this.BGSCodeObj.Confirm(this.TextInput_tf.text);
		}

		public function onHide():void
		{
			trace("[TextInput] onHide");
		}

		public function onShow():void
		{
			trace("[TextInput] onShow");
		}

		public function ProcessUserEvent(asEvent:String, bData:Boolean):Boolean
		{
			trace("[TextInput] ProcessUserEvent: asEvent= " + asEvent + ", bData= " + bData);
			if (!bData)
			{
				switch (asEvent)
				{
					case "Cancel":
						this.onCancelPressed();
						return true;
					case "AButton":
					case "Accept":
						this.onConfirmPressed();
						return true;
				}
			}
			else
			{
				switch (asEvent)
				{
					case "Cancel":
						this.onCancelPressed();
				}
			}
			return false;
		}

		public function onKeyDown(asEvent:KeyboardEvent):*
		{
			trace("[TextInput] onKeyDown: asEvent= " + asEvent);
			switch (asEvent.keyCode)
			{
				case Keyboard.TAB:
					this.onCancelPressed();
					return;
				case Keyboard.ENTER:
					this.onConfirmPressed();
					return;
			}
		}

		public function onKeyUp(asEvent:KeyboardEvent):*
		{
			trace("[TextInput] onKeyUp: asEvent= " + asEvent);
			if (asEvent.keyCode == Keyboard.ESCAPE) this.onCancelPressed();
		}
	}

}
