// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Parameters } from './Parameters';

export interface Converter<T, U, P extends Parameters = Parameters> {
    accepts?(value: T): boolean;
    rejects?(value: T): boolean;
    handles?(params: P): boolean;
    resolve(value: T, params: P): U;
}
