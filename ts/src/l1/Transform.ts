// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Parameters } from './Parameters';

export interface Transform<T, P extends Parameters = Parameters> {
    accepts?(value: T): boolean;
    rejects?(value: T): boolean;
    handles?(params: P): boolean;
    apply(value: T, params: P): T;
}
