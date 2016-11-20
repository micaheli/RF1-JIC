﻿/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Martin Hentschel <info@cl-soft.de>
 *
 */

using System;
using CCodeGeneration;

namespace LwipSnmpCodeGeneration
{
	public class SnmpScalarNodeTruthValue : SnmpScalarNodeInt
	{
		public SnmpScalarNodeTruthValue(SnmpTreeNode parentNode)
			: base(parentNode)
		{
		}

		protected override void GenerateGetMethodCodeCore(CodeContainerBase container, string localValueVarName, ref bool localValueVarUsed, string retLenVarName)
		{
			container.AddCodeFormat("snmp_encode_truthvalue({0}, /* TODO: put requested bool value here */ 0);", localValueVarName);
			localValueVarUsed = true;
			
			container.AddCode(String.Format("{0} = {1};",
				retLenVarName,
				(!String.IsNullOrWhiteSpace(this.FixedValueLength)) ? this.FixedValueLength : "0"));
		}

		protected override void GenerateSetMethodCodeCore(CodeContainerBase container, string localValueVarName, ref bool localValueVarUsed, string lenVarName, ref bool lenVarUsed, string retErrVarName)
		{
			VariableType truthVar = new VariableType("bool_value", LwipDefs.Vt_U8);
			container.Declarations.Add(new VariableDeclaration(truthVar));

			container.AddCodeFormat("snmp_decode_truthvalue({0}, &{1});", localValueVarName, truthVar.Name);
			localValueVarUsed = true;

			container.AddElement(new Comment(String.Format("TODO: store new value contained in '{0}' here", truthVar.Name), singleLine: true)); 
		}
	}
}
