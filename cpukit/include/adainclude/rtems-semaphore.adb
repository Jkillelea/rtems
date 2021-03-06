-- SPDX-License-Identifier: BSD-2-Clause

--
--  RTEMS / Body
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--
--
--  DEPENDENCIES:
--
--
--
--  COPYRIGHT (c) 1997-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

package body RTEMS.Semaphore is

   --
   -- Semaphore Manager
   --

   procedure Create
     (Name             : in RTEMS.Name;
      Count            : in RTEMS.Unsigned32;
      Attribute_Set    : in RTEMS.Attribute;
      Priority_Ceiling : in RTEMS.Tasks.Priority;
      ID               : out RTEMS.ID;
      Result           : out RTEMS.Status_Codes)
   is
      function Create_Base
        (Name             : RTEMS.Name;
         Count            : RTEMS.Unsigned32;
         Attribute_Set    : RTEMS.Attribute;
         Priority_Ceiling : RTEMS.Tasks.Priority;
         ID               : access RTEMS.ID)
         return             RTEMS.Status_Codes;
      pragma Import (C, Create_Base, "rtems_semaphore_create");
      ID_Base : aliased RTEMS.ID;
   begin

      Result :=
         Create_Base
           (Name,
            Count,
            Attribute_Set,
            Priority_Ceiling,
            ID_Base'Access);
      ID     := ID_Base;

   end Create;

   procedure Delete
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Delete_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Delete_Base, "rtems_semaphore_delete");
   begin

      Result := Delete_Base (ID);

   end Delete;

   procedure Ident
     (Name   : in RTEMS.Name;
      Node   : in RTEMS.Unsigned32;
      ID     : out RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Ident_Base
        (Name : RTEMS.Name;
         Node : RTEMS.Unsigned32;
         ID   : access RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Ident_Base, "rtems_semaphore_ident");
      ID_Base : aliased RTEMS.ID;
   begin

      Result := Ident_Base (Name, Node, ID_Base'Access);
      ID     := ID_Base;

   end Ident;

   procedure Obtain
     (ID         : in RTEMS.ID;
      Option_Set : in RTEMS.Option;
      Timeout    : in RTEMS.Interval;
      Result     : out RTEMS.Status_Codes)
   is
      function Obtain_Base
        (ID         : RTEMS.ID;
         Option_Set : RTEMS.Option;
         Timeout    : RTEMS.Interval)
         return       RTEMS.Status_Codes;
      pragma Import (C, Obtain_Base, "rtems_semaphore_obtain");
   begin

      Result := Obtain_Base (ID, Option_Set, Timeout);

   end Obtain;

   procedure Release
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Release_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Release_Base, "rtems_semaphore_release");
   begin

      Result := Release_Base (ID);

   end Release;

   procedure Flush
     (ID     : in RTEMS.ID;
      Result : out RTEMS.Status_Codes)
   is
      function Flush_Base
        (ID   : RTEMS.ID)
         return RTEMS.Status_Codes;
      pragma Import (C, Flush_Base, "rtems_semaphore_flush");
   begin

      Result := Flush_Base (ID);

   end Flush;

end RTEMS.Semaphore;
